#include "Lexer.h"
#include "../debug/Debug.h"
#include <cctype>
#include <iostream>

Lexer::Lexer(const std::string &source) 
    : source(source), file(""), pos(-1), line(1), m_hasError(false)
    , m_keywords
    ({
        { "if", Token(Token::If, "if", file, 0) },
        { "for", Token(Token::For, "for", file, 0) },
        { "else", Token(Token::Else, "else", file, 0) },
        { "while", Token(Token::While, "while", file, 0) },
        { "break", Token(Token::Break, "break", file, 0) },
        { "return", Token(Token::Return, "return", file, 0) },
        { "extern", Token(Token::Extern, "extern", file, 0) },
        { "continue", Token(Token::Continue, "continue", file, 0) },
        { "int", Token(Token::DT_int, "int", file, 0) },
        { "char", Token(Token::DT_char, "char", file, 0) },
        { "float", Token(Token::DT_float, "float", file, 0) },
        { "void", Token(Token::Void, "void", file, 0) }
    })
    {
        this->source += '\0';
    }

std::vector<Token> Lexer::exec()
{
    std::vector<Token> tokens;

    char ch;
    while(ch = next())
    {
        switch (ch)
        {
        //  #include
        case '#':
            handle_include();
            break;
        //  ;
        case ';':
            tokens.push_back(Token(Token::Semicolon, ";", file, line));
        break;
        //  ,
        case ',':
            tokens.push_back(Token(Token::Comma, ",", file, line));
        break;
        //  +
        case '+': 
            tokens.push_back(Token(Token::Plus, "+", file, line));
        break;

        //  -
        case '-': 
            tokens.push_back(Token(Token::Minus, "-", file, line));
        break;

        //  *
        case '*': 
            tokens.push_back(Token(Token::Mult, "*", file, line));
        break;

        //  /
        case '/': 
            tokens.push_back(Token(Token::Div, "/", file, line));
        break;

        //  %
        case '%': 
            tokens.push_back(Token(Token::Mod, "%", file, line));
        break;

        //  (
        case '(': 
            tokens.push_back(Token(Token::OpenParen, "(", file, line));
        break;

        //  )
        case ')': 
            tokens.push_back(Token(Token::CloseParen, ")", file, line));
        break;

        //  [
        case '[': 
            tokens.push_back(Token(Token::OpenSquare, "[", file, line));
        break;

        //  ]
        case ']': 
            tokens.push_back(Token(Token::CloseSquare, "]", file, line));
        break;
        //  {
        case '{': 
            tokens.push_back(Token(Token::OpenBrance, "{", file, line));
        break;

        //  }
        case '}': 
            tokens.push_back(Token(Token::CloseBrance, "}", file, line));
        break;

        //  =   ==
        case '=': 
            if (peek_next() == '=')
            {
                tokens.push_back(Token(Token::Eq_Eq, "==", file, line));
                pos ++;
            }
            else 
            {
                tokens.push_back(Token(Token::Eq, "=", file, line));
            }
        break;
        
        //  !   !=
        case '!': 
            if (peek_next() == '=')
            {
                tokens.push_back(Token(Token::Not_Eq, "!=", file, line));
                pos ++;
            }
            else 
            {
                tokens.push_back(Token(Token::Not, "!", file, line));
            }
        break;

        //  >   >=
        case '>': 
            if (peek_next() == '=')
            {
                tokens.push_back(Token(Token::Greater_Eq, ">=", file, line));
                pos ++;
            }
            else 
            {
                tokens.push_back(Token(Token::Greater, ">", file, line));
            }
        break;

        //  <   <=
        case '<': 
            if (peek_next() == '=')
            {
                tokens.push_back(Token(Token::Less_Eq, "<=", file, line));
                pos ++;
            }
            else 
            {
                tokens.push_back(Token(Token::Less, "<", file, line));
            }
        break;

        //  &   &&
        case '&': 
            if (peek_next() == '&')
            {
                tokens.push_back(Token(Token::And, "&&", file, line));
                pos ++;
            }
            else
            {
                tokens.push_back(Token(Token::Ampersand, "&", file, line));
            }
        break;

        // ||
        case '|':
            if (peek_next() == '|')
            {
                tokens.push_back(Token(Token::Or, "||", file, line));
                pos ++;
            }
            else
            {
                Debug::LexicalError(Token(Token::Identifier, std::string() + ch, file, line));
                m_hasError = true;
            }
        break;

        //  identifier
        case '_':
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm':
        case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M':
        case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
            tokens.push_back(getIdentifier());
            pos --;
        break;

        //  interger & float
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': 
            tokens.push_back(getIntFloat());
            pos --;
        break;

        //  character & string
        case '\'': case '\"': 
            tokens.push_back(getCharStr());
        break;

        //  white
        case ' ': case '\t': case '\n':
            skip_white();
            pos --;
        break;
        default: 
            Debug::LexicalError(Token(Token::Identifier, std::string() + ch, file, line));
            m_hasError = true;
        }
    }
    
    tokens.push_back(Token(Token::Eof, "$", file, line));

    return tokens;
}

bool Lexer::hasError() const
{
    return m_hasError;
}

char Lexer::next()
{
    return source[++ pos];
}

char Lexer::peek_next() const
{
    return source[pos + 1];
}

char Lexer::peek_pre() const
{
    return pos == 0 ? '\n' : source[pos - 1];
}

void Lexer::skip_white()
{
    char ch = source[pos];
    do {
        if (ch == '\n')
        {
            line ++;
        }
        ch = next();
    } while (ch == ' ' || ch == '\t' || ch == '\n');
}

void Lexer::handle_include()
{
    if (peek_pre() != '\n')
    {
        Debug::LexicalError(Token(Token::Identifier, "#", file, line));
        m_hasError = true;
        return;
    }
    // # 1 "main.c"
    std::string row;
    std::string file_name;
    next();
    char ch = next();
    while (ch != ' ')
    {
        row += ch;
        ch = next();
    }
    next();
    ch = next();
    while (ch != '\"')
    {
        file_name += ch;
        ch = next();
    }
    line = std::stoi(row);
    file = file_name;
}

Token Lexer::getIdentifier()
{
    std::string id = "";
    int ch = source[pos];
    
    do {
        id += ch;
        ch = next();
    } while (ch == '_' || isalpha(ch) || isdigit(ch));

    auto it = m_keywords.find(id);
    Token token;

    if (it != m_keywords.end())
    {
        token = it->second;
        token.setLine(line);
    }
    else
    {
        token = Token(Token::Identifier, id, file, line);
    }
    return token;
}

Token Lexer::getIntFloat()
{
    std::string con = "";
    int ch = source[pos];
    int dot = 0;
    bool hasAlpha = false;
    
    do {
        if (isalpha(ch))
        {
            hasAlpha = true;
        }
        else if (ch == '.')
        {
            dot ++;
        }
        con += ch;
        ch = next();
    } while (isalpha(ch) || isdigit(ch) || ch == '.');
    
    Token token(dot ? Token::Float : Token::Integer, con, file, line);

    if (dot > 1 || hasAlpha || con[0] == '0' && con.length() > 1 && isdigit(con[1]))
    {
        Debug::LexicalError(token);
        m_hasError = true;
    }

    return token;
}

Token Lexer::getCharStr()
{
    std::string ch_s = "";
    bool escape_error = false;
    int ch = source[pos];
    char mark = ch;

    do {
        if (ch == '\\' && mark == '\'')
        {
            char nextCh = peek_next();
            /* \n */
            if (nextCh == 'n')
            {
                ch = '\n';
                pos ++;
            }
            /* \0 */
            else if (nextCh == '0')
            {
                ch = '\0';
                pos ++;
            }
            /* \' */
            else if (nextCh == '\'')
            {
                ch = '\'';
                pos ++;
            }
            /* \" */
            else if (nextCh == '\"')
            {
                ch = '\"';
                pos ++;
            }
            /* \\ */
            else if (nextCh == '\\')
            {
                pos ++;
            }
            else
            {
                escape_error = true;
            }
        }
        else if (ch == '\\' && mark == '\"')
        {
            char nextCh = peek_next();
            if (nextCh == '\"')
            {
                ch_s += ch;
                ch_s += next();
                ch = next();
                continue;
            }
        }
        ch_s += ch;
        ch = next();
    } while (ch != mark && ch != '\n');


    if (ch == mark)
    {
        ch_s += ch;
    }

    Token token(mark == '\'' ? Token::Character : Token::String, ch_s, file, line);

    if (ch != mark || escape_error || (mark == '\'' ? ch_s.length() != 3 : ch_s.length() < 2))
    {
        Debug::LexicalError(token);
        m_hasError = true;
    }

    if (ch == '\n')
    {
        line ++;
    }

    return token;
}
