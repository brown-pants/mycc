#include "ASGenerator.h"
#include <limits>

ASGenerator::ASGenerator(const std::vector<TACGenerator::Quaternion> &tac)
    : tac(tac), mOffset(0), paramOffset(8), sub_rsp_pos(0)
{

}

std::string ASGenerator::exec()
{
    dec_mycc_putchar();
    for (const TACGenerator::Quaternion &code : tac)
    {
        switch(code.op)
        {
        case TACGenerator::Op_global_var:
            dec_global_var(code.result, code.arg1);
            break;
        case TACGenerator::Op_local_var:
            dec_local_var(code.result, code.arg1);
            break;
        case TACGenerator::Op_begin_func:
            begin_func(code.result);
            break;
        case TACGenerator::Op_end_func:
            end_func(code.result);
            break;
        case TACGenerator::Op_dec_param:
            dec_param(code.result);
            break;
        case TACGenerator::Op_param:
            param(code.result);
            break;
        case TACGenerator::Op_label:
            label(code.result);
            break;
        case TACGenerator::Op_goto:
            goto_label(code.result);
            break;
        case TACGenerator::Op_if:
            if_goto(code.arg1, code.result);
            break;
        case TACGenerator::Op_assign:
            assign(code.arg1, code.result);
            break;
        case TACGenerator::Op_call_func:
            call_func(code.arg1, code.result);
            break;
        case TACGenerator::Op_add:
            arithmetic("+", code.arg1, code.arg2, code.result);
            break;
        case TACGenerator::Op_sub:
            arithmetic("-", code.arg1, code.arg2, code.result);
            break;
        case TACGenerator::Op_mult:
            arithmetic("*", code.arg1, code.arg2, code.result);
            break;
        case TACGenerator::Op_div:
            arithmetic("/", code.arg1, code.arg2, code.result);
            break;
        case TACGenerator::Op_mod:
            arithmetic("%", code.arg1, code.arg2, code.result);
            break;
        case TACGenerator::Op_eq_eq:
            relational("==", code.arg1, code.arg2, code.result);
            break;
        case TACGenerator::Op_not_eq:
            relational("!=", code.arg1, code.arg2, code.result);
            break;
        case TACGenerator::Op_greater:
            relational(">", code.arg1, code.arg2, code.result);
            break;
        case TACGenerator::Op_greater_eq:
            relational(">=", code.arg1, code.arg2, code.result);
            break;
        case TACGenerator::Op_less:
            relational("<", code.arg1, code.arg2, code.result);
            break;
        case TACGenerator::Op_less_eq:
            relational("<=", code.arg1, code.arg2, code.result);
            break;
        }
    }
    dec_start();
    return asc;
}

std::string ASGenerator::getVarCode(const std::string &var, bool isWrite)
{
    if (var == "~ret")
    {
        return "%rax";
    }
    size_t found = var.find('[');
    // array
    if (found != std::string::npos)
    {
        static int counter = 0;
        std::string var_name = var.substr(0, found);
        std::string idx = var.substr(found + 1, var.size() - found - 2);
        std::string idx_code;
        std::string arr_code = addr_map[var_name];
        std::string mov = "movq";

        if (isNumber(idx))
        {
            idx_code = "$" + idx;
            if (isOutOfInt32Range(std::stoll(idx)))
            {
                mov = "movabsq";
            }
        }
        else
        {
            idx_code = addr_map[idx];
        }
        asc += "\t" + mov + " " + idx_code + ", %rdx\n";        //      {movq | movabsq} {idx_code}, %rdx
        asc += "\tleaq " + arr_code + ", %rcx\n";               //      leaq {arr_code}, %rcx
        
        if (isWrite)
        {
            return "(%rdx, %rcx)";
        }
        
        std::string idx_temp = "idx~" + std::to_string(counter ++);
        dec_local_var(idx_temp, "8");
        std::string idx_temp_code = addr_map[idx_temp];
        asc += "\tmovq (%rdx, %rcx), %rbx\n";                   //      movq (%rdx, %rcx), %rbx
        asc += "\tmovq %rbx, " + idx_temp_code + "\n";          //      movq %rbx, {idx_temp}
        return idx_temp_code;
    }
    // normal variable
    return addr_map[var];
}

void ASGenerator::dec_global_var(const std::string &var_name, const std::string &size)
{
    asc += "\t.global " + var_name + "\n";  //      .global {var_name}
    asc += "\t.bss\n";                      //      .bss
    asc += var_name + ":\n";                // {var_name}:
    asc += "\t.zero " + size + "\n";       //      .zero {size}
    addr_map.insert(std::pair<std::string, std::string>(var_name, var_name + "(%rip)"));    // {var_name} : {var_name}(%rip)
}

void ASGenerator::dec_local_var(const std::string &var_name, const std::string &size)
{
    mOffset -= std::stoll(size);
    addr_map.insert(std::pair<std::string, std::string>(var_name, std::to_string(mOffset) + "(%rbp)"));    // {var_name} : {mOffset}(%rbp)
    localVar_set.insert(var_name);
}

void ASGenerator::begin_func(const std::string &func_name)
{
    asc += ".text\n";        
    asc += func_name + ":\n";               // {func_name}:
    asc += "\tpushq %rbp\n";                //      pushq %rbp
    asc += "\tmovq %rsp, %rbp\n";           //      movq %rsp, %rbp
    sub_rsp_pos = asc.length();
}

void ASGenerator::end_func(const std::string &func_name)
{
    asc.insert(sub_rsp_pos, "\tsubq $" + std::to_string(-mOffset) + ", %rsp\n");      //      subq ${-mOffset}, %rsp
    asc += "\tleave\n";                                         //      {func_name}:
    asc += "\tretq\n";                                          //      retq
    // clear
    for (const std::string &localVar : localVar_set)
    {
        addr_map.erase(localVar);
    }
    localVar_set.clear();
    mOffset = 0;
    paramOffset = 8;
}

void ASGenerator::dec_param(const std::string &param_name)
{
    paramOffset += 8;
    addr_map.insert(std::pair<std::string, std::string>(param_name, std::to_string(paramOffset) + "(%rbp)"));    // {var_name} : {mOffset}(%rbp)
    localVar_set.insert(param_name);
}

void ASGenerator::param(const std::string &param_name)
{
    // param_name is a number
    if (isNumber(param_name))
    {
        // out of int32 range
        if (isNumber(param_name) && isOutOfInt32Range(std::stoll(param_name)))
        {
            asc += "\tmovabsq $" + param_name + ", %rax\n";     //      movabsq ${int64}, %rax
            asc += "\tpushq %rax\n";                            //      pushq %rax
        }
        else
        {
            asc += "\tpushq $" + param_name + "\n";             //      pushq ${int32}
        }
    }
    // param_name is a variable
    else
    {
        asc += "\tpushq " + getVarCode(param_name) + "\n";  //      pushq {addr}
    }
}

void ASGenerator::label(const std::string &label_name)
{
    asc += label_name + ":\n";      // {label_name}:
}

void ASGenerator::goto_label(const std::string &label_name)
{
    asc += "\tjmp " + label_name + "\n";
}

void ASGenerator::if_goto(const std::string &condition, const std::string &label)
{
    // condition is a number
    if (isNumber(condition))
    {
        if (std::stoll(condition))
        {
            goto_label(label);
        }
    }
    //condition is a variable
    else 
    {
        asc += "\tcmpq $0, " + getVarCode(condition) + "\n";    //      cmpq $0, {condition}
        asc += "\tjne " + label + "\n";                         //      jne {label}
    }
}

void ASGenerator::assign(const std::string &arg, const std::string &result)
{
    // result is undefined temp
    if (result.find('[') == std::string::npos && addr_map.find(result) == addr_map.end())
    {
        dec_local_var(result, "8");
    }
    // arg is a number
    if (isNumber(arg))
    {
        // out of int32 range
        if (isNumber(arg) && isOutOfInt32Range(std::stoll(arg)))
        {
            asc += "\tmovabsq $" + arg + ", %rax\n";                            //      movabsq ${int64}, %rax
            asc += "\tmovq %rax, " + getVarCode(result, true) + "\n";           //      movq %rax, {result}
        }
        else 
        {
            asc += "\tmovq $" + arg + ", " + getVarCode(result, true) + "\n";   //      movq ${int32}, {result}
        }
    }
    // arg is a variable
    else
    {
        asc += "\tmovq " + getVarCode(arg) + ", %rax\n";                 //      movq {arg}, %rax
        asc += "\tmovq %rax, " + getVarCode(result, true) + "\n";        //      movq %rax, {result}
    }
}

void ASGenerator::call_func(const std::string &params_count, const std::string &func_name)
{
    asc += "\tcall " + func_name + "\n";                                                //      call {func_name}
    asc += "\taddq $" + std::to_string(std::stoll(params_count) * 8) + ", %rsp\n";      //      addq ${params_size}, %rsp
}

void ASGenerator::arithmetic(const std::string &op, const std::string &arg1, const std::string &arg2, const std::string &result)
{
    bool sign1 = false;
    bool sign2 = false;
    long long num1;
    long long num2;
    // undefined temp
    if (result.find('[') == std::string::npos && addr_map.find(result) == addr_map.end())
    {
        dec_local_var(result, "8");
    }
    // arg1 is a number
    if (isNumber(arg1))
    {
        num1 = std::stoll(arg1);
        sign1 = true;
    }
    // arg2 is a number
    if (isNumber(arg2))
    {
        num2 = std::stoll(arg2);
        sign2 = true;
    }
    // At least one of arg1 and arg2 is not number
    std::string code1, code2, result_code;
    std::string mov1 = "movq", mov2 = "movq";
    if (sign1)
    {
        code1 = "$" + std::to_string(num1);
        if (isOutOfInt32Range(num1))
        {
            mov1 = "movabsq";
        }
    }
    else
    {
        code1 = getVarCode(arg1);
    }
    if (sign2)
    {
        code2 = "$" + std::to_string(num2);
        if (isOutOfInt32Range(num2))
        {
            mov2 = "movabsq";
        }
    }
    else
    {
        code2 = getVarCode(arg2);
    }
    std::string as_op;
    if (op == "+")
    {
        as_op = "addq";
    }
    else if (op == "-")
    {
        as_op = "subq";
    }
    else if (op == "*")
    {
        as_op = "imulq";
    }
    // idivq
    if (op == "/" || op == "%")
    {
        result_code = getVarCode(result);
        asc += "\t" + mov1 + " " + code1 + ", %rax\n";                  //      {movq | movabsq} {arg1}, %rax
        asc += "\tcqto\n";                                              //      cqto
        asc += "\t" + mov2 + " " + code2 + ", %rcx\n";                  //      {movq | movabsq} {arg2}, %rcx
        asc += "\tidivq %rcx\n";                                        //      idivq %rcx
        // quotient
        if(op == "/")
        {
            asc += "\tmovq %rax, " + result_code + "\n";                    //      movq %rax, {result}
        }
        // remainder
        else
        {
            asc += "\tmovq %rdx, " + result_code + "\n";                    //      movq %rdx, {result}
        }
    }
    // addq, subq, imulq
    else
    {
        result_code = getVarCode(result);
        asc += "\t" + mov1 + " " + code1 + ", %rax\n";                  //      {movq | movabsq} {arg1}, %rax
        asc += "\t" + as_op + " " + code2 + ", %rax\n";                 //      {as_op} {arg2}, %rax
        asc += "\tmovq %rax, " + result_code + "\n";                    //      movq %rax, {result}
    }
}

void ASGenerator::relational(const std::string &op, const std::string &arg1, const std::string &arg2, const std::string &result)
{
    bool sign1 = false;
    bool sign2 = false;
    long long num1;
    long long num2;
    // undefined temp
    if (result.find('[') == std::string::npos && addr_map.find(result) == addr_map.end())
    {
        dec_local_var(result, "8");
    }
    // arg1 is a number
    if (isNumber(arg1))
    {
        num1 = std::stoll(arg1);
        sign1 = true;
    }
    // arg2 is a number
    if (isNumber(arg2))
    {
        num2 = std::stoll(arg2);
        sign2 = true;
    }
    // At least one of arg1 and arg2 is not number
    std::string code1, code2, result_code;
    std::string mov1 = "movq", mov2 = "movq";
    if (sign1)
    {
        code1 = "$" + std::to_string(num1);
        if (isOutOfInt32Range(num1))
        {
            mov1 = "movabsq";
        }
    }
    else
    {
        code1 = getVarCode(arg1);
    }
    if (sign2)
    {
        code2 = "$" + std::to_string(num2);
        if (isOutOfInt32Range(num2))
        {
            mov2 = "movabsq";
        }
    }
    else
    {
        code2 = getVarCode(arg2);
    }
    std::string as_op;
    if (op == "==")
    {
        as_op = "sete";
    }
    else if (op == "!=")
    {
        as_op = "setne";
    }
    else if (op == ">=")
    {
        as_op = "setnl";
    }
    else if (op == ">")
    {
        as_op = "setg";
    }
    else if (op == "<=")
    {
        as_op = "setng";
    }
    else if (op == "<")
    {
        as_op = "setl";
    }
    asc += "\t" + mov1 + " " + code1 + ", %rax\n";      //      {movq | movabsq} {arg1}, %rax
    asc += "\t" + mov2 + " " + code2 + ", %rbx\n";      //      {movq | movabsq} {arg2}, %rbx
    asc += "\tcmp %rbx, %rax\n";                        //      cmp %rbx, %rax
    asc += "\t" + as_op + " %cl\n";                     //      {as_op} %cl
    asc += "\tmovzbq %cl, %rax\n";                      //      movzbq %cl, %rax
    asc += "\tmovq %rax, " + addr_map[result] + "\n";   //      movq %rax, {result}
}

void ASGenerator::dec_mycc_putchar()
{
    asc +=  ".text\n"
            "mycc_putchar:\n"
            "\tpushq %rbp\n"
            "\tmovq %rsp, %rbp\n"
            "\tmovq $1, %rax\n"
            "\tmovq $1, %rdi\n"
            "\tleaq 16(%rbp), %rsi\n"
            "\tmovq $1, %rdx\n"
            "\tsyscall\n"
            "\tleave\n"
            "\tretq\n\n";
}

void ASGenerator::dec_start()
{
    asc +=  ".global _start\n"
            "_start:\n"
            "\tcall main\n"
            "\tmovq $60, %rax\n"
            "\txorq %rdi, %rdi\n"
            "\tsyscall\n";
}

bool ASGenerator::isOutOfInt32Range(int64_t number) const
{
    return number < std::numeric_limits<int32_t>::min() || 
           number > std::numeric_limits<int32_t>::max();
}

bool ASGenerator::isNumber(const std::string &str) const
{
    return str[0] == '-' || std::isdigit(str[0]);
}