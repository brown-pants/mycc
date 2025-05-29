#ifndef __TACGENERATOR_H__
#define __TACGENERATOR_H__

#include "../../parse/Parser.h"
#include "../../symtab/Symbol.h"
#include <string>
#include <stack>
#include <unordered_map>

class TACGenerator
{
public:
    enum OpType
    {
        Op_global_var,  // define a global variable : arg1 = size, result = varName
        Op_local_var,   // define a local variable : arg1 = size, result = varName
        Op_begin_func,  // begin of function : result = funcName
        Op_end_func,    // end of function : result = funcName
        Op_dec_param,   // define a param : arg1 = size, result = paramName
        Op_param,       // pasing parameters : param result
        Op_label,       // result = label_name
        Op_goto,        // goto result
        Op_if,          // if arg1 goto result
        Op_assign,      // result = arg1
        Op_call_func,   // arg1 = paramsCount, result = funcName
        Op_add,         // result = arg1 + arg2
        Op_sub,         // result = arg1 - arg2
        Op_mult,        // result = arg1 * arg2
        Op_div,         // result = arg1 / arg2
        Op_eq_eq,       // result = arg1 == arg2
        Op_not_eq,      // result = arg1 != arg2
        Op_greater,     // result = arg1 > arg2
        Op_greater_eq,  // result = arg1 >= arg2
        Op_less,        // result = arg1 < arg2
        Op_less_eq      // result = arg1 <= arg2
    };

    struct Quaternion
    {
        OpType op;
        std::string arg1;
        std::string arg2;
        std::string result;
    };

    TACGenerator(const Parser::TreeNode &root);
    const std::vector<Quaternion> &exec();

    bool hasError() const;

private:
    const Parser::TreeNode &root;
    unsigned int temp_counter;
    unsigned int lable_counter;
    unsigned int scope_counter;
    std::string curFunName;
    bool m_hasError;

    std::stack<std::string> scopeStack;
    std::unordered_map<Token::Type, unsigned int> dataSizeMap;
    std::vector<Quaternion> code;

    std::string new_temp();
    std::string new_lable();
    Symbol *getSymbol(const std::string &symbol_name);

    void generate_3ac(const Parser::TreeNode &node);
    void dec_var(bool local, const Token &type, const Token &id, bool isArray = false, int arrSize = 1);
    void dec_function(const Token &type, const Token &id, const Parser::TreeNode &dec_tail);
    void dec_params(const Parser::TreeNode &node, std::vector<Token::Type> &params_type);
    std::string do_expression(const Parser::TreeNode &node);
    int pasing_params(const Parser::TreeNode &node);
};

#endif