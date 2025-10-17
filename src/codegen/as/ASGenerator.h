#ifndef __ASGENERATOR_H__
#define __ASGENERATOR_H__

#include "RegAllocator.h"
#include <set>
#include <unordered_set>

class ASGenerator
{
public:
    ASGenerator(const std::vector<TACGenerator::Quaternion> &tac);
    std::string exec();

private:
    using RegId = RegAllocator::RegId;
    RegAllocator regAllocator;

    struct VarSymbol
    {
        VarSymbol() {}
        VarSymbol(const std::string &addr, const std::string &type)
            : addr(addr), type(type) {}
        std::string addr;
        std::string type;
    };
    const std::vector<TACGenerator::Quaternion> &tac;
    std::unordered_set<std::string> localVar_set;
    std::unordered_map<std::string, VarSymbol> symTable;
    std::string asc;
    std::string ret_type;
    
    int mOffset;
    int paramOffset;
    int sub_rsp_pos;
    bool has_main;

    std::string getVarCode(const std::string &var, bool isWrite = false);
    void dec_global_init(const std::string &var_name, const std::string &value, const std::string &type);
    void dec_global_var(const std::string &var_name, const std::string &size, const std::string &type);
    void dec_extern_var(const std::string &var_name, const std::string &type);
    void dec_string(const std::string &str_name, const std::string &string);
    void dec_local_var(const std::string &var_name, const std::string &size, const std::string &type);
    void begin_func(const std::string &func_name);
    void end_func(const std::string &func_name);
    void dec_param(const std::string &param_name, const std::string &size, const std::string &type);
    void param(const std::string &param_name);
    void label(const std::string &label_name);
    void goto_label(const std::string &label_name);
    void if_goto(const std::string &condition, const std::string &label);
    void assign(const std::string &arg, const std::string &result);
    void call_func(const std::string &params_count, const std::string &return_type, const std::string &func_name);
    void logic_not(const std::string &arg, const std::string &result);
    void negate(const std::string &arg, const std::string &result);
    void address(const std::string &arg, const std::string &result);
    void reference(const std::string &arg, const std::string &result);
    void arithmetic(const std::string &op, const std::string &arg1, const std::string &arg2, const std::string &result);
    void relational(const std::string &op, const std::string &arg1, const std::string &arg2, const std::string &result);

    void dec_start();

    bool isOutOfInt32Range(int64_t number) const;
    bool isNumber(const std::string &str) const;
    bool isRegister(const std::string &str) const;
    bool isOneByteType(const std::string &str) const;
    std::string getSymbolType(const std::string &str) const;
    void setRegister(const std::string &var_name, RegId regid, const std::string &type);

    std::map<RegId, bool> m_tempRegs;
    std::set<RegId> m_ignoreRegs;
    RegId getTempReg();
    void freeTempRegs(RegId ignore = RegId::Nul);

    std::set<RegId> m_currentVarRegs;
    int save_reg_pos;
    int call_func_end_pos;
};

#endif