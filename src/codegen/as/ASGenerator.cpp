#include "ASGenerator.h"

ASGenerator::ASGenerator(const std::vector<TACGenerator::Quaternion> &tac)
    : tac(tac), mOffset(0), paramOffset(8)
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
            dec_param(code.result, code.arg1);
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
        std::string idx_code = addr_map[idx];
        std::string arr_code = addr_map[var_name];
        asc += "\tmovq " + idx_code + ", %rdx\n";               //      movq {idx_code}, %rdx
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
    asc += "\tsubq $" + size + ", %rsp\n";      //      subq ${size}, %rsp
    mOffset -= std::stoi(size);
    addr_map.insert(std::pair<std::string, std::string>(var_name, std::to_string(mOffset) + "(%rbp)"));    // {var_name} : {mOffset}(%rbp)
    localVar_set.insert(var_name);
}

void ASGenerator::begin_func(const std::string &func_name)
{
    asc += ".text\n";        
    asc += func_name + ":\n";               // {func_name}:
    asc += "\tpushq %rbp\n";                //      pushq %rbp
    asc += "\tmovq %rsp, %rbp\n";           //      movq %rsp, %rbp
}

void ASGenerator::end_func(const std::string &func_name)
{
    asc += "\taddq $" + std::to_string(-mOffset) + ", %rsp\n";  //      addq ${-mOffset}, %rsp
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

void ASGenerator::dec_param(const std::string &param_name, const std::string &size)
{
    paramOffset += 8;
    asc += "\tpushq " + std::to_string(paramOffset) + "(%rbp)\n";    //      pushq {paramOffset}(%rbp)mOffset -= std::stoi(size);
    mOffset -= std::stoi(size);
    addr_map.insert(std::pair<std::string, std::string>(param_name, std::to_string(mOffset) + "(%rbp)"));    // {var_name} : {mOffset}(%rbp)
    localVar_set.insert(param_name);
}

void ASGenerator::param(const std::string &param_name)
{
    // param_name is a interage or a character
    if (std::isdigit(param_name[0]) || param_name[0] == '\'')
    {
        asc += "\tpushq $" + param_name + "\n";             //      pushq ${interage | character}
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
    // condition is a interage
    if (std::isdigit(condition[0]))
    {
        if (std::stoi(condition))
        {
            goto_label(label);
        }
    }
    //condition is a character
    else if (condition[0] == '\'')
    {
        if (condition[1])
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
    // undefined temp
    if (result.find('[') == std::string::npos && addr_map.find(result) == addr_map.end())
    {
        dec_local_var(result, "8");
    }
    // arg is a interage or a character
    if (std::isdigit(arg[0]) || arg[0] == '\'')
    {
        asc += "\tmovq $" + arg + ", " + getVarCode(result, true) + "\n";         //      movq ${interage | character}, {result}
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
}

void ASGenerator::arithmetic(const std::string &op, const std::string &arg1, const std::string &arg2, const std::string &result)
{
    bool sign1 = false;
    bool sign2 = false;
    int num1;
    int num2;
    // undefined temp
    if (result.find('[') == std::string::npos && addr_map.find(result) == addr_map.end())
    {
        dec_local_var(result, "8");
    }
    // arg1 is a interage
    if (std::isdigit(arg1[0]))
    {
        num1 = std::stoi(arg1);
        sign1 = true;
    }
    // arg1 is a character
    else if (arg1[0] == '\'')
    {
        num1 = arg1[1];
        sign1 = true;
    }
    // arg2 is a interage
    if (std::isdigit(arg2[0]))
    {
        num2 = std::stoi(arg2);
        sign2 = true;
    }
    // arg2 is a character
    else if (arg2[0] == '\'')
    {
        num2 = arg2[1];
        sign2 = true;
    }
    // Both of arg1 and arg2 is number
    if (sign1 && sign2)
    {
        int num;
        if (op == "+")
        {
            num = num1 + num2;
        }
        else if (op == "-")
        {
            num = num1 - num2;
        }
        else if (op == "*")
        {
            num = num1 * num2;
        }
        else if (op == "/")
        {
            num = num1 / num2;
        }
        asc += "\tmovq $" + std::to_string(num) + ", " + getVarCode(result) + "\n";    //      movq ${num}, {result}
        return;
    }
    // At least one of arg1 and arg2 is not number
    std::string code1, code2, result_code;
    if (sign1)
    {
        code1 = "$" + std::to_string(num1);
    }
    else
    {
        code1 = getVarCode(arg1);
    }
    if (sign2)
    {
        code2 = "$" + std::to_string(num2);
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
    else if (op == "/")
    {
        as_op = "idivq";
    }
    // idivq
    if (op == "/")
    {
        result_code = getVarCode(result);
        asc += "\tmovq " + code1 + ", %rax\n";                          //      movq {arg1}, %rax
        asc += "\tcqto\n";                                              //      cqto
        asc += "\tmovq " + code2 + ", %rcx\n";                          //      movq {arg2}, %rcx
        asc += "\tdivq %rcx\n";                                         //      divq %rcx
        asc += "\tmovq %rax, " + result_code + "\n";                    //      movq %rax, {result}
    }
    // addq, subq, imulq
    else
    {
        result_code = getVarCode(result);
        asc += "\tmovq " + code1 + ", %rax\n";                          //      movq {arg1}, %rax
        asc += "\t" + as_op + " " + code2 + ", %rax\n";                 //      {as_op} {arg2}, %rax
        asc += "\tmovq %rax, " + result_code + "\n";                    //      movq %rax, {result}
    }
}

void ASGenerator::relational(const std::string &op, const std::string &arg1, const std::string &arg2, const std::string &result)
{
    bool sign1 = false;
    bool sign2 = false;
    int num1;
    int num2;
    // undefined temp
    if (result.find('[') == std::string::npos && addr_map.find(result) == addr_map.end())
    {
        dec_local_var(result, "8");
    }
    // arg1 is a interage
    if (std::isdigit(arg1[0]))
    {
        num1 = std::stoi(arg1);
        sign1 = true;
    }
    // arg1 is a character
    else if (arg1[0] == '\'')
    {
        num1 = arg1[1];
        sign1 = true;
    }
    // arg2 is a interage
    if (std::isdigit(arg2[0]))
    {
        num2 = std::stoi(arg2);
        sign2 = true;
    }
    // arg2 is a character
    else if (arg2[0] == '\'')
    {
        num2 = arg2[1];
        sign2 = true;
    }
    // Both of arg1 and arg2 is number
    if (sign1 && sign2)
    {
        int condition;
        if (op == "==")
        {
            condition = num1 == num2;
        }
        else if (op == "!=")
        {
            condition = num1 != num2;
        }
        else if (op == ">=")
        {
            condition = num1 >= num2;
        }
        else if (op == ">")
        {
            condition = num1 > num2;
        }
        else if (op == "<=")
        {
            condition = num1 <= num2;
        }
        else if (op == "<")
        {
            condition = num1 < num2;
        }
        asc += "\tmovq $" + std::to_string(condition) + ", " + getVarCode(result) + "\n";    //      movq ${condition}, {result}
        return;
    }
    // At least one of arg1 and arg2 is not number
    std::string code1, code2, result_code;
    if (sign1)
    {
        code1 = "$" + std::to_string(num1);
    }
    else
    {
        code1 = getVarCode(arg1);
    }
    if (sign2)
    {
        code2 = "$" + std::to_string(num2);
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
    asc += "\tmovq " + code1 + ", %rax\n";              //      movq {arg1}, %rax
    asc += "\tmovq " + code2 + ", %rbx\n";              //      movq {arg2}, %rbx
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
            "\tpushq 16(%rbp)\n"
            "\tmovq $1, %rax\n"
            "\tmovq $1, %rdi\n"
            "\tleaq -8(%rbp), %rsi\n"
            "\tmovq $1, %rdx\n"
            "\tsyscall\n"
            "\taddq $8, %rsp\n"
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
