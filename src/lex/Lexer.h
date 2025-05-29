#ifndef __LEXER_H__
#define __LEXER_H__

#include "../token/Token.h"
#include <vector>
#include <unordered_map>

class Lexer
{
public:
    Lexer(const std::string &source);

    std::vector<Token> exec();

    bool hasError() const;

private:
    std::unordered_map<std::string, Token> m_keywords;
    std::string source;
    int pos;
    int line;
    bool m_hasError;

    char next();
    char peek_next() const;
    void skip_white();
    
    Token getIdentifier();
    Token getIntFloat();
    Token getCharStr();
};

#endif