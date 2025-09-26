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
            dec_global_var(code.result, code.arg1, code.arg2);
            break;
        case TACGenerator::Op_local_var:
            dec_local_var(code.result, code.arg1, code.arg2);
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
            call_func(code.arg1, code.arg2, code.result);
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
    if (var[0] == '*')
    {
        std::string var_name = var.substr(1);
        std::string addr = symTable[var_name].addr;
        asc += "\tmovq " + addr + ", %rbx\n";  // movq {addr}, %rbx
        if (isWrite)
        {
            return "(%rbx)";
        }
        static int counter = 0;
        std::string ref_temp = "ref~" + std::to_string(counter ++);
        dec_local_var(ref_temp, isOneByteType(var) ? "1" : "8", getSymbolType(var));
        std::string ref_temp_code = symTable[ref_temp].addr;
        asc += "\tmovq (%rbx), %rbx\n";                 // movq (%rbx), %rbx
        asc += "\tmovq %rbx, " + ref_temp_code + "\n";  // movq %rbx, {ref_temp_code}
        return ref_temp_code;
    }
    if (var[0] == '&')
    {
        std::string var_name = var.substr(1);
        std::string addr = symTable[var_name].addr;
        static int counter = 0;
        std::string ptr_temp = "ptr~" + std::to_string(counter ++);
        dec_local_var(ptr_temp, "8", getSymbolType(var));
        std::string ptr_temp_code = symTable[ptr_temp].addr;
        asc += "\tleaq " + addr + ", %rax\n";           // leaq {addr}, %rax
        asc += "\tmovq %rax, " + ptr_temp_code + "\n";  // movq %rax, {ptr_temp_code}
        return ptr_temp_code;
    }
    size_t found = var.find('[');
    // array
    if (found != std::string::npos)
    {
        std::string var_name = var.substr(0, found);
        std::string idx = var.substr(found + 1, var.size() - found - 2);
        std::string idx_code;
        std::string arr_code = symTable[var_name].addr;
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
            idx_code = symTable[idx].addr;
        }
        asc += "\t" + mov + " " + idx_code + ", %rdx\n";        //      {movq | movabsq} {idx_code}, %rdx
        asc += "\tleaq " + arr_code + ", %rbx\n";               //      leaq {arr_code}, %rbx
        
        if (isWrite)
        {
            return "(%rbx, %rdx)";
        }
        
        static int counter = 0;
        std::string idx_temp = "idx~" + std::to_string(counter ++);
        dec_local_var(idx_temp, "8", getSymbolType(var));
        std::string idx_temp_code = symTable[idx_temp].addr;
        asc += "\tmovq (%rbx, %rdx), %rbx\n";                   //      movq {addr}, %rbx
        asc += "\tmovq %rbx, " + idx_temp_code + "\n";          //      movq %rbx, {idx_temp}
        return idx_temp_code;
    }
    // normal variable
    return symTable[var].addr;
}

void ASGenerator::dec_global_var(const std::string &var_name, const std::string &size, const std::string &type)
{
    asc += "\t.global " + var_name + "\n";  //      .global {var_name}
    asc += "\t.bss\n";                      //      .bss
    asc += var_name + ":\n";                // {var_name}:
    asc += "\t.zero " + size + "\n";        //      .zero {size}
    symTable.insert(std::pair<std::string, VarSymbol>(var_name, VarSymbol(var_name + "(%rip)", type)));    // {var_name} : {var_name}(%rip)
}

void ASGenerator::dec_local_var(const std::string &var_name, const std::string &size, const std::string &type)
{
    mOffset -= std::stoll(size);
    symTable.insert(std::pair<std::string, VarSymbol>(var_name, VarSymbol(std::to_string(mOffset) + "(%rbp)", type)));    // {var_name} : {mOffset}(%rbp)
    localVar_set.insert(var_name);
}

void ASGenerator::begin_func(const std::string &func_name)
{
    asc += "\t.text\n";        
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
        symTable.erase(localVar);
    }
    localVar_set.clear();
    mOffset = 0;
    paramOffset = 8;
}

void ASGenerator::dec_param(const std::string &param_name, const std::string &type)
{
    paramOffset += 8;
    symTable.insert(std::pair<std::string, VarSymbol>(param_name, VarSymbol(std::to_string(paramOffset) + "(%rbp)", type)));    // {var_name} : {mOffset}(%rbp)
    localVar_set.insert(param_name);
}

void ASGenerator::param(const std::string &param_name)
{
    bool paramOneByte = isOneByteType(param_name);
    // param_name is a number
    if (isNumber(param_name))
    {
        // out of int32 range
        if (isNumber(param_name) && isOutOfInt32Range(std::stoll(param_name)))
        {
            asc += "\tmovabsq $" + param_name + ", %rax\n";         //      movabsq ${int64}, %rax
            asc += "\tpushq %rax\n";                                //      pushq %rax
        }
        else
        {
            asc += "\tpushq $" + param_name + "\n";                 //      pushq ${int32}
        }
    }
    // param_name is a one byte variable
    else if (paramOneByte)
    {
        asc += "\tmovsbq " + getVarCode(param_name) + ", %rax\n";   //      movsbq {addr}, %rax
        asc += "\tpushq %rax\n";                                     //      pushq %rax
    }
    // param_name is a eight bytes variable
    else
    {
        asc += "\tpushq " + getVarCode(param_name) + "\n";          //      pushq {addr}
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
    if (result.substr(0, 2) == "t^")
    {
        std::string argType = getSymbolType(arg);
        std::string argPrefix = argType.substr(0, argType.find('_'));
        if (argPrefix == "ptr")
        {
            dec_local_var(result, "8", argType);
        }
        else
        {
            dec_local_var(result, "8", "var_int");
        }
    }
    bool resultOneByte = isOneByteType(result);
    // arg is a number
    if (isNumber(arg))
    {
        // out of int32 range
        if (isOutOfInt32Range(std::stoll(arg)))
        {
            asc += "\tmovabsq $" + arg + ", %rax\n";                            //      movabsq ${int64}, %rax
            if (resultOneByte)
            {
                asc += "\tmovb %al, " + getVarCode(result, true) + "\n";        //      movb %al, {result}
            }
            else
            {
                asc += "\tmovq %rax, " + getVarCode(result, true) + "\n";       //      movq %rax, {result}
            }
        }
        else if(resultOneByte)
        {
            asc += "\tmovq $" + arg + ", %rax\n";                               //      movq ${int32}, %rax
            asc += "\tmovb %al, " + getVarCode(result, true) + "\n";            //      movb %al, {result}
        }
        else
        {
            asc += "\tmovq $" + arg + ", " + getVarCode(result, true) + "\n";   //      movq ${int32}, {result}
        }
    }
    // arg is a variable
    else
    {
        bool argOneByte = isOneByteType(arg);
        if (argOneByte)
        {
            asc += "\tmovsbq " + getVarCode(arg) + ", %rax\n";                  //      movsbq {arg}, %rax
        }
        else
        {
            asc += "\tmovq " + getVarCode(arg) + ", %rax\n";                    //      movq {arg}, %rax
        }
        if (resultOneByte)
        {
            asc += "\tmovb %al, " + getVarCode(result, true) + "\n";            //      movb %al, {result}
        }
        else
        {
            asc += "\tmovq %rax, " + getVarCode(result, true) + "\n";           //      movq %rax, {result}
        }
    }
}

void ASGenerator::call_func(const std::string &params_count, const std::string &return_type, const std::string &func_name)
{
    asc += "\tcall " + func_name + "\n";                                                //      call {func_name}
    asc += "\taddq $" + std::to_string(std::stoll(params_count) * 8) + ", %rsp\n";      //      addq ${params_size}, %rsp
    ret_type = return_type;
}

void ASGenerator::arithmetic(const std::string &op, const std::string &arg1, const std::string &arg2, const std::string &result)
{
    bool sign1 = false;
    bool sign2 = false;
    long long num1;
    long long num2;
    // undefined temp
    if (result.substr(0, 2) == "t^")
    {
        std::string arg1Type = getSymbolType(arg1);
        std::string arg2Type = getSymbolType(arg2);
        std::string arg1Prefix = arg1Type.substr(0, arg1Type.find('_'));
        std::string arg2Prefix = arg2Type.substr(0, arg2Type.find('_'));
        // arg1 is pointer
        if (arg1Prefix == "ptr")
        {
            dec_local_var(result, "8", arg1Type);
        }
        // arg2 is pointer
        else if (arg2Prefix == "ptr")
        {
            dec_local_var(result, "8", arg2Type);
        }
        else
        {
            dec_local_var(result, "8", "var_int");
        }
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

    bool resultOneByte = isOneByteType(result);
    bool arg1OneByte = isOneByteType(arg1);
    bool arg2OneByte = isOneByteType(arg2);

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
        if (arg1OneByte)
        {
            mov1 = "movsbq";
        }
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
        if (arg2OneByte)
        {
            mov2 = "movsbq";
        }
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
        asc += "\t" + mov1 + " " + code1 + ", %rax\n";                  //      {movq | movabsq | movsbq} {arg1}, %rax
        asc += "\tcqto\n";                                              //      cqto
        asc += "\t" + mov2 + " " + code2 + ", %rcx\n";                  //      {movq | movabsq | movsbq} {arg2}, %rcx
        asc += "\tidivq %rcx\n";                                        //      idivq %rcx
        result_code = getVarCode(result, true);
        // quotient
        if(op == "/")
        {
            if (resultOneByte)
            {
                asc += "\tmovb %al, " + result_code + "\n";             //      movb %al, {result}
            }
            else
            {
                asc += "\tmovq %rax, " + result_code + "\n";            //      movq %rax, {result}
            }
        }
        // remainder
        else
        {
            if (resultOneByte)
            {
                asc += "\tmovb %dl, " + result_code + "\n";             //      movb %dl, {result}
            }
            else
            {
                asc += "\tmovq %rdx, " + result_code + "\n";            //      movq %rdx, {result}
            }
        }
    }
    // addq, subq, imulq
    else
    {
        asc += "\t" + mov1 + " " + code1 + ", %rax\n";                  //      {movq | movabsq | movsbq} {arg1}, %rax
        if (arg2OneByte)
        {
            asc += "\tmovsbq " + code2 + ", %rcx\n";                    //      {movsbq} {arg2}, %rcx
            asc += "\t" + as_op + " %rcx, %rax\n";                      //      {as_op} %rcx, %rax
        }
        else
        {
            asc += "\t" + as_op + " " + code2 + ", %rax\n";             //      {as_op} {arg2}, %rax
        }
        result_code = getVarCode(result, true);
        if (resultOneByte)
        {
            asc += "\tmovb %al, " + result_code + "\n";                 //      movb %al, {result}
        }
        else
        {
            asc += "\tmovq %rax, " + result_code + "\n";                //      movq %rax, {result}
        }
    }
}

void ASGenerator::relational(const std::string &op, const std::string &arg1, const std::string &arg2, const std::string &result)
{
    bool sign1 = false;
    bool sign2 = false;
    long long num1;
    long long num2;
    // undefined temp
    if (result.substr(0, 2) == "t^")
    {
        dec_local_var(result, "8", "var_int");
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

    bool resultOneByte = isOneByteType(result);
    bool arg1OneByte = isOneByteType(arg1);
    bool arg2OneByte = isOneByteType(arg2);

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
        if (arg1OneByte)
        {
            mov1 = "movsbq";
        }
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
        if (arg2OneByte)
        {
            mov2 = "movsbq";
        }
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
    
    asc += "\t" + mov1 + " " + code1 + ", %rax\n";              //      {movq | movabsq | movsbq} {arg1}, %rax
    asc += "\t" + mov2 + " " + code2 + ", %rbx\n";              //      {movq | movabsq | movsbq} {arg2}, %rbx
    asc += "\tcmp %rbx, %rax\n";                                //      cmp %rbx, %rax
    asc += "\t" + as_op + " %cl\n";                             //      {as_op} %cl
    asc += "\tmovzbq %cl, %rax\n";                              //      movzbq %cl, %rax
    if (resultOneByte)
    {
        asc += "\tmovb %al, " + getVarCode(result, true) + "\n";   //      movb %al, {result}
    }
    else
    {
        asc += "\tmovq %rax, " + getVarCode(result, true) + "\n";  //      movq %rax, {result}
    }
}

void ASGenerator::dec_mycc_putchar()
{
    asc +=  "\t.text\n"
            "mycc_putchar:\n"
            "\tpushq %rbp\n"
            "\tmovq %rsp, %rbp\n"
            "\tmovq $1, %rax\n"
            "\tmovq $1, %rdi\n"
            "\tleaq 16(%rbp), %rsi\n"
            "\tmovq $1, %rdx\n"
            "\tsyscall\n"
            "\tleave\n"
            "\tretq\n";
}

void ASGenerator::dec_start()
{
    asc +=  "\t.global _start\n"
            "\t.text\n"
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

bool ASGenerator::isOneByteType(const std::string &str) const
{
    if (isNumber(str))
    {
        return false;
    }
    if (str == "~ret")
    {
        return ret_type == "var_char";
    }
    if (str[0] == '&')
    {
        return false;
    }
    if (str[0] == '*')
    {
        std::string type = symTable.find(str.substr(1))->second.type;
        return type == "arr_char" || type == "ptr_char";
    }
    size_t pos = str.find('[');
    if (pos != std::string::npos)
    {
        std::string type = symTable.find(str.substr(0, pos))->second.type;
        return type == "arr_char" || type == "ptr_char";
    }
    return symTable.find(str)->second.type == "var_char";
}

std::string ASGenerator::getSymbolType(const std::string &str) const
{
    if (isNumber(str))
    {
        return "var_int";
    }
    if (str == "~ret")
    {
        return ret_type;
    }
    if (str[0] == '&')
    {
        std::string type = symTable.find(str.substr(1))->second.type;
        std::string data_type = type.substr(type.find('_'));
        return "ptr" + data_type;
    }
    if (str[0] == '*')
    {
        std::string type = symTable.find(str.substr(1))->second.type;
        std::string data_type = type.substr(type.find('_'));
        return "var" + data_type;
    }
    size_t pos = str.find('[');
    if (pos != std::string::npos)
    {
        std::string type = symTable.find(str.substr(0, pos))->second.type;
        std::string data_type = type.substr(type.find('_'));
        return "var" + data_type;
    }
    return symTable.find(str)->second.type;
}