#ifndef __ASGENERATOR_H__
#define __ASGENERATOR_H__

#include "../3ac/TACGenerator.h"
#include <unordered_set>

class ASGenerator
{
public:
    ASGenerator(const std::vector<TACGenerator::Quaternion> &tac);
    std::string exec();

private:
    const std::vector<TACGenerator::Quaternion> &tac;
    std::unordered_set<std::string> localVar_set;
    std::unordered_map<std::string, std::string> addr_map;
    std::string asc;
    int mOffset;
    int paramOffset;
    int sub_rsp_pos;

    std::string getVarCode(const std::string &var, bool isWrite = false);
    void dec_global_var(const std::string &var_name, const std::string &size);
    void dec_local_var(const std::string &var_name, const std::string &size);
    void begin_func(const std::string &func_name);
    void end_func(const std::string &func_name);
    void dec_param(const std::string &param_name, const std::string &size);
    void param(const std::string &param_name);
    void label(const std::string &label_name);
    void goto_label(const std::string &label_name);
    void if_goto(const std::string &condition, const std::string &label);
    void assign(const std::string &arg, const std::string &result);
    void call_func(const std::string &params_count, const std::string &func_name);
    void arithmetic(const std::string &op, const std::string &arg1, const std::string &arg2, const std::string &result);
    void relational(const std::string &op, const std::string &arg1, const std::string &arg2, const std::string &result);

    void dec_mycc_putchar();
    void dec_start();
};

#endif