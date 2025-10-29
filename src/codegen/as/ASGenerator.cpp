#include "ASGenerator.h"
#include <limits>

ASGenerator::ASGenerator(const std::vector<TACGenerator::Quaternion> &tac)
    : regAllocator(tac), tac(tac), mOffset(0), paramOffset(8), sub_rsp_pos(0), has_main(false), save_reg_pos(-1), call_func_end_pos(0)
{
    regAllocator.exec();
}

std::string ASGenerator::exec()
{
    for (int idx = 0; idx < tac.size(); idx ++)
    {
        const TACGenerator::Quaternion &code = tac[idx];
        if (regAllocator.isInvalidVar(code.result))
        {
            continue;
        }
        switch(code.op)
        {
        case TACGenerator::Op_global_init:
            dec_global_init(code.result, code.arg1, code.arg2);
            break;
        case TACGenerator::Op_global_var:
            dec_global_var(code.result, code.arg1, code.arg2);
            break;
        case TACGenerator::Op_extern_var:
            dec_extern_var(code.result, code.arg1);
            break;
        case TACGenerator::Op_dec_string:
            dec_string(code.result, code.arg1);
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
            dec_param(code.result, code.arg1, code.arg2);
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
        case TACGenerator::Op_not:
            logic_not(code.arg1, code.result);
            break;
        case TACGenerator::Op_neg:
            negate(code.arg1, code.result);
            break;
        case TACGenerator::Op_address:
            address(code.arg1, code.result);
            break;
        case TACGenerator::Op_ref:
            reference(code.arg1, code.result);
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
        // free var regs
        auto regs = regAllocator.freeVarRegs(idx);
        for (auto reg : regs)
        {
            m_currentVarRegs.erase(reg);
        }
    }
    if (has_main)
    {
        dec_start();
    }
    return asc;
}

std::string ASGenerator::getVarCode(const std::string &var, bool isWrite)
{
    if (var == "~ret")
    {
        return "%rax";
    }
    // normal variable
    auto iter = symTable.find(var);
    if (iter != symTable.end())
    {
        return iter->second.addr;
    }
    return "";
}

void ASGenerator::dec_global_init(const std::string &var_name, const std::string &value, const std::string &type)
{
    std::string result = value;
    if (value[0] == '&')
    {
        result = value.substr(1);
    }
    std::string as_type = (type == "var_char" ? ".byte" : ".quad");
    asc += "\t.global " + var_name + "\n";          //      .global {var_name}
    asc += "\t.data\n";                             //      .data
    asc += var_name + ":\n";                        // {var_name}:
    asc += "\t" + as_type + " " + result + "\n";    //      {.byte | .quad} {result}
    symTable.insert(std::pair<std::string, VarSymbol>(var_name, VarSymbol(var_name + "(%rip)", type)));    // {var_name} : {var_name}(%rip)
}

void ASGenerator::dec_global_var(const std::string &var_name, const std::string &size, const std::string &type)
{
    asc += "\t.global " + var_name + "\n";  //      .global {var_name}
    asc += "\t.bss\n";                      //      .bss
    asc += var_name + ":\n";                // {var_name}:
    asc += "\t.zero " + size + "\n";        //      .zero {size}
    symTable.insert(std::pair<std::string, VarSymbol>(var_name, VarSymbol(var_name + "(%rip)", type)));    // {var_name} : {var_name}(%rip)
}

void ASGenerator::dec_extern_var(const std::string &var_name, const std::string &type)
{
    symTable.insert(std::pair<std::string, VarSymbol>(var_name, VarSymbol(var_name + "(%rip)", type)));    // {var_name} : {var_name}(%rip)
}

void ASGenerator::dec_string(const std::string &str_name, const std::string &string)
{
    asc += "\t.section .rodata\n";          //      .section .rodata
    asc += str_name + ":\n";                // {str_name}:
    asc += "\t.string " + string + "\n";    //      {.string} {string}
    symTable.insert(std::pair<std::string, VarSymbol>(str_name, VarSymbol(str_name + "(%rip)", "arr_char")));    // {str_name} : {str_name}(%rip)
}

void ASGenerator::dec_local_var(const std::string &var_name, const std::string &size, const std::string &type)
{
    mOffset -= std::stoll(size);
    symTable.insert(std::pair<std::string, VarSymbol>(var_name, VarSymbol(std::to_string(mOffset) + "(%rbp)", type)));    // {var_name} : {mOffset}(%rbp)
    localVar_set.insert(var_name);
}

void ASGenerator::begin_func(const std::string &func_name)
{
    if (func_name == "main")
    {
        has_main = true;
    }
    asc += "\t.global " + func_name + "\n"; //      .global {func_name}
    asc += "\t.text\n";        
    asc += func_name + ":\n";               // {func_name}:
    asc += "\tpushq %rbp\n";                //      pushq %rbp
    asc += "\tmovq %rsp, %rbp\n";           //      movq %rsp, %rbp
    sub_rsp_pos = asc.length();
}

void ASGenerator::end_func(const std::string &func_name)
{
    if (mOffset != 0)
    {  
        asc.insert(sub_rsp_pos, "\tsubq $" + std::to_string(-mOffset) + ", %rsp\n");      //      subq ${-mOffset}, %rsp
    }
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

void ASGenerator::dec_param(const std::string &param_name, const std::string &size, const std::string &type)
{
    paramOffset += 8;
    dec_local_var(param_name, size, type);
    std::string addr = getVarCode(param_name);

    RegId regid = getTempReg();
    std::string reg64 = regAllocator.toReg64(regid);
    std::string reg8 = regAllocator.toReg8(regid);

    asc += "\tmovq " + std::to_string(paramOffset) + "(%rbp)" + ", " + reg64 + "\n";    //      movq {mOffset}(%rbp), {reg64}
    if (isOneByteType(param_name))
    {
        asc += "\tmovb " + reg8 + ", " + addr + "\n";                                   //      movb {reg8}, {param}
    }
    else
    {
        asc += "\tmovq " + reg64 + ", " + addr + "\n";                                  //      movq {reg64}, {param}
    }
    freeTempRegs();
}

void ASGenerator::param(const std::string &param_name)
{
    if (save_reg_pos < 0)
    {
        save_reg_pos = asc.length();
    }

    std::string param_code = getVarCode(param_name);
    bool paramOneByte = isOneByteType(param_name);

    // param_name is a number
    if (isNumber(param_name))
    {
        // out of int32 range
        if (isOutOfInt32Range(std::stoll(param_name)))
        {
            RegId regid = getTempReg();
            std::string reg64 = regAllocator.toReg64(regid);
            asc += "\tmovabsq $" + param_name + ", " + reg64 + "\n";        //      movabsq ${int64}, {reg64}
            asc += "\tpushq " + reg64 + "\n";                               //      pushq {reg64}
        }
        else
        {
            asc += "\tpushq $" + param_name + "\n";                         //      pushq ${int32}
        }
    }
    // param_name is a one byte variable and is not a register
    else if (paramOneByte && !isRegister(param_code))
    {
        RegId regid = getTempReg();
        std::string reg64 = regAllocator.toReg64(regid);
        asc += "\tmovsbq " + param_code + ", " + reg64 + "\n";              //      movsbq {addr}, {reg64}
        asc += "\tpushq " + reg64 + "\n";                                   //      pushq {reg64}
    }
    // param_name is a eight bytes variable
    else
    {
        asc += "\tpushq " + param_code + "\n";                              //      pushq {addr}
    }

    freeTempRegs();
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
    std::string result_code = getVarCode(condition);
    bool oneByte = isOneByteType(condition);
    if (isRegister(result_code))
    {
        if (oneByte)
        {
            RegId regid = regAllocator.getRegId(result_code);
            std::string reg8 = regAllocator.toReg8(regid);
            asc += "\ttestb " + reg8 + ", " + reg8 + "\n";                  //      testb {reg8}, {reg8}
        }
        else
        {
            asc += "\ttestq " + result_code + ", " + result_code + "\n";    //      testq {condition}, {condition}
        }
    }
    else
    {
        RegId regid = getTempReg();
        if (oneByte)
        {
            std::string reg8 = regAllocator.toReg8(regid);
            asc += "\tmovb " + result_code + ", " + reg8 + "\n";             //      movb {condition}, {reg8}
            asc += "\ttestb " + reg8 + ", " + reg8 + "\n";                  //      testb {reg8}, {reg8}
        }
        else
        {
            std::string reg64 = regAllocator.toReg64(regid);
            asc += "\tmovq " + result_code + ", " + reg64 + "\n";           //      movq {condition}, {reg64}
            asc += "\ttestq " + reg64 + ", " + reg64 + "\n";                //      testq {reg64}, {reg64}
        }
        freeTempRegs();
    }
    asc += "\tjnz " + label + "\n";                                         //      jnz {label}
}

void ASGenerator::assign(const std::string &arg, const std::string &result)
{
    // result is undefined temp
    if (result.substr(0, 2) == "t^")
    {
        std::string argType = getSymbolType(arg);
        std::string argPrefix = argType.substr(0, argType.find('_'));
        RegId regid = regAllocator.getVarReg(result);
        if (regid == RegId::Nul)
        {
            dec_local_var(result, "8", argPrefix == "ptr" ? argType : "var_int");
        }
        else
        {
            setRegister(result, regid, argPrefix == "ptr" ? argType : "var_int");
        }
    }

    std::string result_code;
    // result is a reference
    if (result[0] == '*')
    {
        std::string ptr_name = result.substr(1);
        std::string ptr_code = getVarCode(ptr_name);
        // (reg) -> *ptr
        if (isRegister(ptr_code))
        {
            result_code = '(' + ptr_code + ')';
            m_ignoreRegs.insert(regAllocator.getRegId(ptr_code));
        }
        else
        {
            RegId regid;
            regid = getTempReg();
            std::string regStr = regAllocator.toReg64(regid);
            asc += "\tmovq " + ptr_code + ", " + regStr + "\n";     // movq {ptr_code}, {regStr}
            result_code = '(' + regStr + ')';
        }
    }
    else
    {
        result_code = getVarCode(result);
    }

    if (arg == "~ret")
    {
        asc.insert(call_func_end_pos, "\tmovq %rax, " + result_code + "\n");
        return;
    }
    
    std::string arg_code = getVarCode(arg);
    bool argOneByte = isOneByteType(arg);
    bool resultOneByte = isOneByteType(result);
    // result is a register
    if (isRegister(result_code))
    {
        // arg is a number
        if (isNumber(arg))
        {
            // out of int32 range
            if (isOutOfInt32Range(std::stoll(arg)))
            {
                asc += "\tmovabsq $" + arg + ", " + result_code + "\n";     //      movabsq ${int64}, {result_code}
            }
            else
            {
                asc += "\tmovq $" + arg + ", " + result_code + "\n";        //      movq ${int32}, {result_code}
            }
        }
        // arg is a variable
        else
        {
            if (argOneByte && !isRegister(arg_code))
            {
                RegId regid = regAllocator.getRegId(result_code);
                std::string reg8 = regAllocator.toReg8(regid);
                asc += "\tmovb " + arg_code + ", " + reg8 + "\n";           //      movb {arg_code}, {reg8}
                asc += "\tmovsbq " + reg8 + ", " + result_code + "\n";      //      movsbq {reg8}, {result_code}
            }
            else
            {
                asc += "\tmovq " + arg_code + ", " + result_code + "\n";    //      movq {arg_addr}, {result_addr}
            }
        }
    }
    // result is in memory
    else
    {
        // arg is a number
        if (isNumber(arg))
        {
            // out of int32 range
            if (isOutOfInt32Range(std::stoll(arg)))
            {
                RegId regid = getTempReg();
                std::string reg64 = regAllocator.toReg64(regid);
                std::string reg8 = regAllocator.toReg8(regid);
                asc += "\tmovabsq $" + arg + ", " + reg64 + "\n";               //      movabsq ${int64}, {reg64}
                if (resultOneByte)
                {
                    asc += "\tmovb " + reg8 + ", " + result_code + "\n";        //      movb {reg8}, {result}
                }
                else
                {
                    asc += "\tmovq " + reg64 + ", " + result_code + "\n";       //      movq {reg64}, {result}
                }
            }
            else if(resultOneByte)
            {
                RegId regid = getTempReg();
                std::string reg64 = regAllocator.toReg64(regid);
                std::string reg8 = regAllocator.toReg8(regid);
                asc += "\tmovq $" + arg + ", " + reg64 + "\n";                  //      movq ${int32}, {reg64}
                asc += "\tmovb " + reg8 + ", " + result_code + "\n";            //      movb {reg8}, {result}
            }
            else
            {
                asc += "\tmovq $" + arg + ", " + result_code + "\n";            //      movq ${int32}, {result}
            }
        }
        // arg is a variable
        else
        {
            std::string reg8;
            std::string reg64;
            if (isRegister(arg_code))
            {
                RegId regid = regAllocator.getRegId(arg_code);
                reg64 = regAllocator.toReg64(regid);
                reg8 = regAllocator.toReg8(regid);
            }
            else
            {
                RegId regid = getTempReg();
                reg64 = regAllocator.toReg64(regid);
                reg8 = regAllocator.toReg8(regid);
                if (argOneByte)
                {
                    asc += "\tmovb " + arg_code + ", " + reg8 + "\n";           //      movb {arg}, {reg8}
                    asc += "\tmovsbq " + reg8 + ", " + reg64 + "\n";            //      movsbq {reg8}, {reg64}
                }
                else
                {
                    asc += "\tmovq " + arg_code + ", " + reg64 + "\n";          //      movq {arg}, {reg64}
                }
            }
            if (resultOneByte)
            {
                asc += "\tmovb " + reg8 + ", " + result_code + "\n";            //      movb {reg8}, {result}
            }
            else
            {
                asc += "\tmovq " + reg64 + ", " + result_code + "\n";           //      movq {reg64}, {result}
            }
        }
    }

    freeTempRegs();
}

void ASGenerator::call_func(const std::string &params_count, const std::string &return_type, const std::string &func_name)
{
    std::stack<std::string> regStack;
    for (auto reg : m_currentVarRegs)
    {
        std::string regStr = regAllocator.toReg64(reg);
        regStack.push(regStr);
        std::string code = "\tpushq " + regStr + "\n";  //  pushq {reg}
        // no param
        if (save_reg_pos < 0)
        {
            asc += code;
        }
        else
        {
            asc.insert(save_reg_pos, code);
        }
    }
    
    asc += "\tcall " + func_name + "\n";                                                    //      call {func_name}
    if (params_count != "0")
    {
        asc += "\taddq $" + std::to_string(std::stoll(params_count) * 8) + ", %rsp\n";      //      addq ${params_size}, %rsp
    }

    ret_type = return_type;
    save_reg_pos = -1;
    call_func_end_pos = asc.length();

    while(!regStack.empty())
    {
        std::string regStr = regStack.top();
        asc += "\tpopq " + regStr + "\n";  //  popq {reg}
        regStack.pop();
    }
}

void ASGenerator::logic_not(const std::string &arg, const std::string &result)
{
    // undefined temp
    if (result.substr(0, 2) == "t^")
    {
        RegId regid = regAllocator.getVarReg(result);
        if (regid == RegId::Nul)
        {
            dec_local_var(result, "8", "var_int");
        }
        else
        {
            setRegister(result, regid, "var_int");
        }
    }

    std::string result_code = getVarCode(result, true);
    std::string arg_code = getVarCode(arg);
    std::string arg_64 = arg_code;
    std::string arg_8 = arg_code;

    if (isRegister(arg_code))
    {
        RegId regid = regAllocator.getRegId(arg_code);
        arg_64 = regAllocator.toReg64(regid);
        arg_8 = regAllocator.toReg8(regid);
    }
    
    if (isRegister(result_code))
    {
        RegId regid = regAllocator.getRegId(result_code);
        std::string reg64 = regAllocator.toReg64(regid);
        std::string reg8 = regAllocator.toReg8(regid);
        if (isOneByteType(arg))
        {
            asc += "\tmovb " + arg_8 + ", " + reg8 + "\n";          //  movb {arg}, {reg8}
            asc += "\ttestb " + reg8 + ", " + reg8 + "\n";          //  testb {reg8}, {reg8}
        }
        else
        {
            asc += "\tmovq " + arg_64 + ", " + reg64 + "\n";        //  movq {arg}, {reg64}
            asc += "\ttestq " + reg64 + ", " + reg64 + "\n";        //  testq {reg64}, {reg64}
        }
        asc += "\tsetz " + reg8 + "\n";                             //  setz {reg8}
        asc += "\tmovzbq " + reg8 + ", " + reg64 + "\n";            //  movzbq {reg8}, {reg64}
    }
    // result_code is in memory
    else
    {
        RegId regid = getTempReg();
        std::string reg64 = regAllocator.toReg64(regid);
        std::string reg8 = regAllocator.toReg8(regid);
        if (isOneByteType(arg))
        {
            asc += "\tmovb " + arg_8 + ", " + reg8 + "\n";              //  movb {arg}, {reg8}
            asc += "\ttestb " + reg8 + ", " + reg8 + "\n";              //  testb {reg8}, {reg8}
            asc += "\tsetz " + reg8 + "\n";                             //  setz {reg8}
        }
        else
        {
            asc += "\tmovq " + arg_64 + ", " + reg64 + "\n";            //  movq {arg}, {reg64}
            asc += "\ttestq " + reg64 + ", " + reg64 + "\n";            //  testq {reg64}, {reg64}
            asc += "\tsetz " + reg8 + "\n";                             //  setz {reg8}
        }
        asc += "\tmovzbq " + reg8 + ", " + reg64 + "\n";            //  movzbq {reg8}, {reg64}
        asc += "\tmovq " + reg64 + ", " + result_code + "\n";       //  movq {reg64}, {result}
        
        freeTempRegs();
    }
}

void ASGenerator::negate(const std::string &arg, const std::string &result)
{
    // undefined temp
    if (result.substr(0, 2) == "t^")
    {
        RegId regid = regAllocator.getVarReg(result);
        if (regid == RegId::Nul)
        {
            dec_local_var(result, "8", "var_int");
        }
        else
        {
            setRegister(result, regid, "var_int");
        }
    }

    std::string result_code = getVarCode(result, true);
    std::string arg_code = getVarCode(arg);
    std::string arg_64 = arg_code;
    std::string arg_8 = arg_code;

    if (isRegister(arg_code))
    {
        RegId regid = regAllocator.getRegId(arg_code);
        arg_64 = regAllocator.toReg64(regid);
        arg_8 = regAllocator.toReg8(regid);
    }

    bool isArgOneByte = isOneByteType(arg);
    
    if (isRegister(result_code))
    {
        RegId regid = regAllocator.getRegId(result_code);
        std::string reg64 = regAllocator.toReg64(regid);
        std::string reg8 = regAllocator.toReg8(regid);
        if (isArgOneByte)
        {
            asc += "\tmovb " + arg_8 + ", " + reg8 + "\n";      //  movb {arg}, {reg8}
            asc += "\tnegb " + reg8 + "\n";                     //  negb {reg8}
            asc += "\tmovsbq " + reg8 + ", " + reg64 + "\n";    //  movsbq {reg8}, {reg64}
        }
        else
        {
            asc += "\tmovq " + arg_64 + ", " + reg64 + "\n";    //  movq {arg}, {reg64}
            asc += "\tnegq " + reg64 + "\n";                    //  negq {reg64}
        }
    }
    // result is in memory
    else
    {
        RegId regid = getTempReg();
        std::string reg64 = regAllocator.toReg64(regid);
        std::string reg8 = regAllocator.toReg8(regid);
        if (isArgOneByte)
        {
            asc += "\tmovb " + arg_8 + ", " + reg8 + "\n";          //  movb {arg}, {reg8}
            asc += "\tnegb " + reg8 + "\n";                         //  negb {reg8}
            asc += "\tmovsbq " + reg8 + ", " + reg64 + "\n";        //  movsbq {reg8}, {reg64}
            asc += "\tmovq " + reg64 + ", " + result_code + "\n";   //  movq {reg64}, {result}
        }
        else
        {
            asc += "\tmovq " + arg_64 + ", " + reg64 + "n";         //  movq {arg}, {reg64}
            asc += "\tnegq " + reg64 + "\n";                        //  negq {reg64}
            asc += "\tmovq " + reg64 + ", " + result_code + "\n";   //  movq {reg64}, {result}
        }
        
        freeTempRegs();
    }
}

void ASGenerator::address(const std::string &arg, const std::string &result)
{
    // undefined temp
    if (result.substr(0, 2) == "t^")
    {
        std::string argType = getSymbolType(arg);
        std::string type = "ptr" + argType.substr(argType.find('_'));
        RegId regid = regAllocator.getVarReg(result);
        if (regid == RegId::Nul)
        {
            dec_local_var(result, "8", type);
        }
        else
        {
            setRegister(result, regid, type);
        }
    }

    std::string arg_code = getVarCode(arg);
    std::string result_code = getVarCode(result);

    if (isRegister(result_code))
    {
        asc += "\tleaq " + arg_code + ", " + result_code + "\n";    // leaq {arg_addr}, {result_reg}
    }
    else
    {
        RegId regid = getTempReg();
        std::string regStr = regAllocator.toReg64(regid);
        asc += "\tleaq " + arg_code + ", " + regStr + "\n";         // leaq {arg_addr}, {temp_reg}
        asc += "\tmovq " + regStr + ", " + result_code + "\n";      // movq {temp_reg}, {result_addr}
        freeTempRegs();
    }
}

void ASGenerator::reference(const std::string &arg, const std::string &result)
{
    // undefined temp
    if (result.substr(0, 2) == "t^")
    {
        RegId regid = regAllocator.getVarReg(result);
        if (regid == RegId::Nul)
        {
            dec_local_var(result, "8", "var_int");
        }
        else
        {
            setRegister(result, regid, "var_int");
        }
    }

    std::string arg_code = getVarCode(arg);
    std::string result_code = getVarCode(result);
    std::string mov = "movq";

    bool refOneByte = isOneByteType('*' + arg);
    if (refOneByte)
    {
        mov = "movzbq";
    }

    if (isRegister(arg_code))
    {
        arg_code = '(' + arg_code + ')';
        m_ignoreRegs.insert(regAllocator.getRegId(arg_code));
    }
    else
    {
        RegId regid;
        regid = getTempReg();
        std::string regStr = regAllocator.toReg64(regid);
        asc += "\tmovq " + arg_code + ", " + regStr + "\n";                 // movq {arg_addr}, {regStr}
        arg_code = '(' + regStr + ')';
    }

    if (isRegister(result_code))
    {
        asc += "\t" + mov + " " + arg_code + ", " + result_code + "\n";     // {movq | movzbq} {arg_addr}, {result_reg}
    }
    else
    {
        RegId regid = getTempReg();
        std::string regStr = regAllocator.toReg64(regid);
        asc += "\t" + mov + " " + arg_code + ", " + regStr + "\n";          // {movq | movzbq} {arg_addr}, {regStr}
        asc += "\tmovq " + regStr + ", " + result_code + "\n";              // movq {regStr}, {result_addr}
    }
    
    freeTempRegs();
}

void ASGenerator::arithmetic(const std::string &op, const std::string &arg1, const std::string &arg2, const std::string &result)
{
    // undefined temp
    if (result.substr(0, 2) == "t^")
    {
        std::string arg1Type = getSymbolType(arg1);
        std::string arg2Type = getSymbolType(arg2);
        std::string arg1Prefix = arg1Type.substr(0, arg1Type.find('_'));
        std::string arg2Prefix = arg2Type.substr(0, arg2Type.find('_'));
        RegId regid = regAllocator.getVarReg(result);
        std::string type;
        // arg1 is pointer
        if (arg1Prefix == "ptr")
        {
            type = arg1Type;
        }
        // arg2 is pointer
        else if (arg2Prefix == "ptr")
        {
            type = arg2Type;
        }
        else
        {
            type = "var_int";
        }
        if (regid == RegId::Nul)
        {
            dec_local_var(result, "8", type);
        }
        else
        {
            setRegister(result, regid, type);
        }
    }

    bool sign1 = false;
    bool sign2 = false;
    long long num1;
    long long num2;
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
    std::string code1, code2, result_code = getVarCode(result);
    std::string mov1 = "movq", mov2 = "movq";

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
            if (isRegister(code1))
            {
                RegId regid = regAllocator.getRegId(code1);
                code1 = regAllocator.toReg8(regid);
            }
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
            if (isRegister(code2))
            {
                RegId regid = regAllocator.getRegId(code2);
                code2 = regAllocator.toReg8(regid);
            }
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
        RegId raxId = regAllocator.getRegId("%rax");
        RegId rdxId = regAllocator.getRegId("%rdx");
        bool isRaxFree = regAllocator.isFreeReg(raxId);
        bool isRdxFree = regAllocator.isFreeReg(rdxId);
        if (code2 == "%rax" || code2 == "%rdx" || !isRegister(code2) || mov2 != "movq")
        {
            m_ignoreRegs.insert(raxId);
            m_ignoreRegs.insert(rdxId);
            m_ignoreRegs.insert(regAllocator.getRegId(code1));
            RegId regid = getTempReg();
            std::string reg64 = regAllocator.toReg64(regid);
            asc += "\t" + mov2 + " " + code2 + ", " + reg64 + "\n";     //  {movq | movabsq | movsbq} {arg2}, {reg64}
            code2 = reg64;
        }
        int pushRegPos = asc.length();
        asc += "\t" + mov1 + " " + code1 + ", %rax\n";      //  {movq | movabsq | movsbq} {arg1}, %rax
        asc += "\tcqto\n";                                  //  cqto
        asc += "\tidivq " + code2 + "\n";                   //  idivq {arg2}
        // quotient
        if(op == "/")
        {
            asc += "\tmovq %rax, " + result_code + "\n";    //  movq %rax, {result}
        }
        // remainder
        else
        {
            asc += "\tmovq %rdx, " + result_code + "\n";    //  movq %rdx, {result}
        }
        // popq
        if (result_code != "%rax" && !isRaxFree)
        {
            asc.insert(pushRegPos, "\tpushq %rax\n");       //  push %rax
            asc += "\tpopq %rax\n";                         //  popq %rax
        }
        if (result_code != "%rdx" && !isRdxFree)
        {
            asc.insert(pushRegPos, "\tpushq %rdx\n");       //  push %rdx
            asc += "\tpopq %rdx\n";                         //  popq %rdx
        }
    }
    // addq, subq, imulq
    else
    {
        if (isRegister(result_code))
        {
            asc += "\t" + mov1 + " " + code1 + ", " + result_code + "\n";                  //      {movq | movabsq | movsbq} {arg1}, {result}
            if (mov2 == "movsbq" || mov2 == "movabsq")
            {
                m_ignoreRegs.insert(regAllocator.getRegId(result_code));
                RegId regid = getTempReg();
                std::string reg2 = regAllocator.toReg64(regid);
                asc += "\t" + mov2 + " " + code2 + ", " + reg2 + "\n";                     //      {movsbq | movabsq} {arg2}, {reg2}
                asc += "\t" + as_op + " " + reg2 + ", " + result_code + "\n";              //      {as_op} {reg2}, {result}
            }
            else
            {
                asc += "\t" + as_op + " " + code2 + ", " + result_code + "\n";              //      {as_op} {arg2}, {result}
            }
        }
        // result_code is in memory
        else
        {
            m_ignoreRegs.insert(regAllocator.getRegId(code2));
            RegId regid = getTempReg();
            std::string reg1 = regAllocator.toReg64(regid);
            asc += "\t" + mov1 + " " + code1 + ", " + reg1 + "\n";                      //      {movq | movabsq | movsbq} {arg1}, {reg1}
            if (mov2 == "movsbq" || mov2 == "movabsq")
            {
                RegId regid = getTempReg();
                std::string reg2 = regAllocator.toReg64(regid);
                asc += "\t" + mov2 + " " + code2 + ", " + reg2 + "\n";                  //      {movsbq | movabsq} {arg2}, {reg2}
                asc += "\t" + as_op + " " + reg2 + ", " + reg1 + "\n";                  //      {as_op} {reg2}, {reg1}
            }
            else
            {
                asc += "\t" + as_op + " " + code2 + ", " + reg1 + "\n";                 //      {as_op} {arg2}, {reg1}
            }
            asc += "\tmovq " + reg1 + ", " + result_code + "\n";                        //      movq {reg1}, {result}
        }
        
    }
    freeTempRegs(regAllocator.getRegId(result_code));
}

void ASGenerator::relational(const std::string &op, const std::string &arg1, const std::string &arg2, const std::string &result)
{
    // undefined temp
    if (result.substr(0, 2) == "t^")
    {
        RegId regid = regAllocator.getVarReg(result);
        if (regid == RegId::Nul)
        {
            dec_local_var(result, "8", "var_int");
        }
        else
        {
            setRegister(result, regid, "var_int");
        }
    }

    bool sign1 = false;
    bool sign2 = false;
    long long num1;
    long long num2;
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
    std::string code1, code2;
    std::string mov1 = "movq", mov2 = "movq";

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
            if (isRegister(code1))
            {
                RegId regid = regAllocator.getRegId(code1);
                code1 = regAllocator.toReg8(regid);
            }
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
            if (isRegister(code2))
            {
                RegId regid = regAllocator.getRegId(code2);
                code2 = regAllocator.toReg8(regid);
            }
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

    std::string result_code = getVarCode(result, true);
    
    if (isRegister(result_code))
    {
        RegId regid = regAllocator.getRegId(result_code);
        std::string result_64 = regAllocator.toReg64(regid);
        std::string result_8 = regAllocator.toReg8(regid);
        asc += "\t" + mov1 + " " + code1 + ", " + result_64 + "\n";     //      {movq | movabsq | movsbq} {arg1}, {result_64}
        if (mov2 == "movsbq" || mov2 == "movabsq")
        {
            m_ignoreRegs.insert(regAllocator.getRegId(result_code));
            RegId regid = getTempReg();
            std::string reg64 = regAllocator.toReg64(regid);
            asc += "\t" + mov2 + " " + code2 + ", " + reg64 + "\n";     //      {movsbq | movabsq} {arg2}, {reg64}
            code2 = reg64;
        }
        asc += "\tcmpq " + code2 + ", " + result_64 + "\n";             //      cmpq {arg2}, {result_64}
        asc += "\t" + as_op + " " + result_8 + "\n";                    //      {as_op} {result_8}
        asc += "\tmovzbq " + result_8 + ", " + result_64 + "\n";        //      movzbq {result_8}, {result_64}
    }
    // result is in memory
    else
    {
        m_ignoreRegs.insert(regAllocator.getRegId(code2));
        RegId regid = getTempReg();
        std::string reg1_64 = regAllocator.toReg64(regid);
        std::string reg1_8 = regAllocator.toReg8(regid);
        asc += "\t" + mov1 + " " + code1 + ", " + reg1_64 + "\n";       //      {movq | movabsq | movsbq} {arg1}, {reg1_64}
        if (mov2 == "movsbq" || mov2 == "movabsq")
        {
            RegId regid = getTempReg();
            std::string reg64 = regAllocator.toReg64(regid);
            asc += "\t" + mov2 + " " + code2 + ", " + reg64 + "\n";     //      {movsbq | movabsq} {arg2}, {reg64}
            code2 = reg64;
        }
        asc += "\tcmpq " + code2 + ", " + reg1_64 + "\n";               //      cmpq {arg2}, {reg1_64}
        asc += "\t" + as_op + " " + reg1_8 + "\n";                      //      {as_op} {reg1_8}
        asc += "\tmovzbq " + reg1_8 + ", " + reg1_64 + "\n";            //      movzbq {reg1_8}, {reg1_64}
        asc += "\tmovq " + reg1_64 + ", " + result_code + "\n";         //      movq {reg1_64}, {result}
    }

    freeTempRegs();
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

bool ASGenerator::isRegister(const std::string &str) const
{
    return str[0] == '%';
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
        return type == "ptr_char";
    }
    std::string type = symTable.find(str)->second.type;
    return type == "var_char" || type == "arr_char";
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
    return symTable.find(str)->second.type;
}

void ASGenerator::setRegister(const std::string &var_name, RegId regid, const std::string &type)
{
    std::string regStr = regAllocator.toReg64(regid);
    symTable.insert(std::pair<std::string, VarSymbol>(var_name, VarSymbol(regStr, type)));    // {var_name} : {reg}
    localVar_set.insert(var_name);
    m_currentVarRegs.insert(regid);
}

ASGenerator::RegId ASGenerator::getTempReg()
{
    RegId reg = regAllocator.getOneReg();
    if (reg != RegId::Nul)
    {
        m_tempRegs[reg] = false;
        return reg;
    }
    for (int i = 0; i < RegAllocator::reg_count; i ++)
    {
        RegId reg = RegId(i);
        if (m_tempRegs.find(reg) == m_tempRegs.end() && m_ignoreRegs.find(reg) == m_ignoreRegs.end())
        {
            std::string regStr = regAllocator.toReg64(reg);
            asc += "\tpushq " + regStr + "\n";          // pushq {regStr}
            m_tempRegs[reg] = true;
            return reg;
        }
    }
    return RegId::Nul;
}

void ASGenerator::freeTempRegs(RegId ignore)
{
    for (auto [regid, memory] : m_tempRegs)
    {
        if (memory)
        {
            if (regid != ignore)
            {
                std::string regStr = regAllocator.toReg64(regid);
                asc += "\tpopq " + regStr + "\n";       // popq {regStr}
            }
            else
            {
                asc += "\tsubq $8, %rsp\n";             // subq $8, %rsp
            }
        }
        else
        {
            regAllocator.freeReg(regid);
        }
    }
    m_tempRegs.clear();
    m_ignoreRegs.clear();
}