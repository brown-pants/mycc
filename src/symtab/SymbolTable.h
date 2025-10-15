#ifndef __SYMBOLTABLE_H__
#define __SYMBOLTABLE_H__

#include "Symbol.h"
#include <unordered_map>

class SymbolTable
{
public:
    static SymbolTable &GetInstance();
    void insert(const std::string &symbol_name, const std::string &scope, Symbol *symbol);
    void clear();
    Symbol *find(const std::string &symbol_name, const std::string &scope);

private:
    SymbolTable();
    
    //                                 <    scope  , symbol  >
    using ScopeMap = std::unordered_map<std::string, Symbol *>;
    //                                 < symbol_name, ScopeMap>
    using SymbolMap = std::unordered_map<std::string, ScopeMap>;

    SymbolMap symTable;
};

#endif