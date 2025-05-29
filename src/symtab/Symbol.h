#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include "../token/Token.h"
#include <vector>

struct Symbol
{
    enum Type
    {
        Var,
        Arr,
        Func
    }type;

    Symbol(Type type) : type(type) {}
};

struct VarSymbol : public Symbol
{
    VarSymbol(Token::Type data_type, const std::string &newName) : Symbol(Var), data_type(data_type), newName(newName) {}
    Token::Type data_type;
    std::string newName;
};

struct ArrSymbol : public Symbol
{
    ArrSymbol(Token::Type data_type, int size, const std::string &newName) : Symbol(Arr), data_type(data_type), size(size), newName(newName) {}
    Token::Type data_type;
    int size;
    std::string newName;
};

struct FunctionSymbol : public Symbol
{
    FunctionSymbol(Token::Type return_type, const std::vector<Token::Type> &params_type) : Symbol(Func), return_type(return_type), params_type(params_type) {}
    Token::Type return_type;
    std::vector<Token::Type> params_type;
};

#endif