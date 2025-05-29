#include "Token.h"

Token::Token(Type type, const std::string &lexeme, int line)
    : m_type(type), m_lexeme(lexeme), m_line(line) {}
    
void Token::setType(Type type)
{
    this->m_type = type;
}

void Token::setLexeme(const std::string &lexeme)
{
    this->m_lexeme = lexeme;
}

void Token::setLine(int line) 
{ 
    this->m_line = line;
}


Token::Type Token::type() const
{
    return m_type;
}

std::string Token::lexeme() const
{
    return m_lexeme;
}

int Token::line() const
{
    return m_line;
}