#include "SymbolTable.h"

SymbolTable::SymbolTable()
{

}

SymbolTable &SymbolTable::GetInstance()
{
    static SymbolTable instance;
    return instance;
}

void SymbolTable::insert(const std::string &symbol_name, const std::string &scope, Symbol *symbol)
{
    auto iter = symTable.find(symbol_name);
    if (iter == symTable.end())
    {
        symTable.insert(std::pair<std::string, ScopeMap>(symbol_name, ScopeMap()))
        .first->second.insert(std::pair<std::string, Symbol *>(scope, symbol));
    }
    else
    {
        iter->second.insert(std::pair<std::string, Symbol *>(scope, symbol));
    }
}

void SymbolTable::clear()
{
    for (auto mp : symTable)
    {
        for (auto pir : mp.second)
        {
            delete pir.second;
        }
    }
    symTable.clear();
}

Symbol *SymbolTable::find(const std::string &symbol_name, const std::string &scope)
{
    auto iter1 = symTable.find(symbol_name);
    if (iter1 != symTable.end())
    {
        auto iter2 = iter1->second.find(scope);
        if (iter2 != iter1->second.end())
        {
            return iter2->second;
        }
    }
    return nullptr;
}