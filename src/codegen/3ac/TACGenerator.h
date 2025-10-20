#ifndef __TACGENERATOR_H__
#define __TACGENERATOR_H__

#include "../../parse/Parser.h"
#include "../../symtab/Symbol.h"
#include <string>
#include <stack>
#include <unordered_map>
#include <unordered_set>

class TACGenerator
{
public:
    enum OpType
    {
        Op_global_init, // define a global variable and assign an initial value: arg1 = initial_value, arg2 = type, result = varName
        Op_global_var,  // define a global variable : arg1 = size, arg2 = type, result = varName
        Op_extern_var,  // declare a extern var : arg1 = type, result = varName
        Op_dec_string,  // define a string : result = strName, arg1 = string
        Op_local_var,   // define a local variable : arg1 = size, arg2 = type, result = varName
        Op_begin_func,  // begin of function : result = funcName
        Op_end_func,    // end of function : result = funcName
        Op_dec_param,   // define a param : result = paramName, arg1 = size, arg2 = type
        Op_param,       // pasing parameters : param result
        Op_label,       // result = label_name
        Op_goto,        // goto result
        Op_if,          // if arg1 goto result
        Op_assign,      // result = arg1
        Op_call_func,   // arg1 = paramsCount, arg2 = returnType, result = funcName
        Op_not,         // result = !arg1
        Op_neg,         // result = -arg1
        Op_address,     // result = &arg1
        Op_ref,         // result = *arg1
        Op_add,         // result = arg1 + arg2
        Op_sub,         // result = arg1 - arg2
        Op_mult,        // result = arg1 * arg2
        Op_div,         // result = arg1 / arg2
        Op_mod,         // result = arg1 % arg2
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
    unsigned int label_counter;
    unsigned int str_counter;
    unsigned int scope_counter;
    std::string curFunName;
    bool m_hasError;

    std::stack<std::string> scopeStack;
    std::stack<std::string> breakStack;
    std::stack<std::string> continueStack;
    std::unordered_map<Token::Type, unsigned int> dataSizeMap;
    std::unordered_map<std::string, unsigned int> pointerTemps;
    std::unordered_map<std::string, std::string> strNames;
    std::unordered_set<std::string> definedFunctions;
    std::vector<Quaternion> code;
    std::unordered_map<std::string, std::string> tempValues;

    std::string new_temp();
    std::string new_label();
    std::string new_string();
    void handle_star(std::string &var);
    std::string getStrName(const std::string &str);
    Symbol *getSymbol(const std::string &symbol_name);

    void generate_3ac(const Parser::TreeNode &node);
    void dec_var(bool local, const Token &type, const Token &id, const std::string &value = "", const std::string &arr_ptr = "", int arrSize = 1);
    void dec_function(const Token &type, bool returnPointer, const Token &id, const Parser::TreeNode &dec_tail);
    void dec_params(const Parser::TreeNode &node, std::vector<Token::Type> &params_type, bool defineParams = true);
    std::string constant_additive(OpType op, const std::string &arg1, const std::string &arg2);
    std::string do_expression(const Parser::TreeNode &node);
    std::string do_id_tail(const Parser::TreeNode &id_tail, const Token &id);
    int passing_params(const Parser::TreeNode &node);
    
    bool isNumber(const std::string &str) const;
    unsigned int getPointerStride(const std::string &str) const; // if (str is a pointer) return stride; else return 0;
};

#endif