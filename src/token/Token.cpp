#include "Token.h"

Token::Token(Type type, const std::string &lexeme, const std::string &file, int line)
    : m_type(type), m_lexeme(lexeme), m_file(file), m_line(line) {}
    
void Token::setType(Type type)
{
    this->m_type = type;
}

void Token::setLexeme(const std::string &lexeme)
{
    this->m_lexeme = lexeme;
}

void Token::setFile(const std::string &file)
{
    this->m_file = file;
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

std::string Token::file() const
{
    return m_file;
}

int Token::line() const
{
    return m_line;
}