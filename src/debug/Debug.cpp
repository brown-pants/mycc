#include "Debug.h"
#include <iostream>

std::string Debug::curFile;
std::unordered_map<Token::Type, std::string> Debug::TokenTypeMap = 
{
    { Token::Semicolon,     "';'" },
    { Token::Comma,         "','" },
    { Token::Plus,          "'+'" },
    { Token::Minus,         "'-'" },
    { Token::OpenParen,     "'('" },
    { Token::CloseParen,    "')'" },
    { Token::OpenSquare,    "'['" },
    { Token::CloseSquare,   "']'" },
    { Token::OpenBrance,    "'{'" },
    { Token::CloseBrance,   "'}'" },
    { Token::Eq,            "'=" },
    { Token::Eq_Eq,         "'=='" },
    { Token::Not,           "'!'" },
    { Token::Not_Eq,        "'!='" },
    { Token::Greater,       "'>'" },
    { Token::Greater_Eq,    "'>='" },
    { Token::Less,          "'<'" },
    { Token::Less_Eq,       "'<='" },
    { Token::Identifier,    "Identifier" },
    { Token::Integer,       "Integer" },
    { Token::Character,     "Character" },
    { Token::Float,         "Float" },
    { Token::If,            "if" },
    { Token::For,           "for" },
    { Token::Else,          "else" },
    { Token::While,         "while" },
    { Token::Return,        "return" },
    { Token::DT_int,        "int" },
    { Token::DT_char,       "char" },
    { Token::DT_float,      "float" },
    { Token::Void,          "void" },
    { Token::Eof,           "$" },
    { Token::Nul,           "Nul" }
};

Debug::Debug() {}

const char *red = "\033[31m";
const char *white = "\033[0m";
const char *green = "\033[32m";

void Debug::NoInput()
{
    std::cout << "mycc: " << red << "fatal error: " << white << "no input files" << std::endl;
    std::cout << "compilation terminated." << std::endl;
}

void Debug::LexicalError(const Token &token)
{
    std::cout << curFile << ":" << token.line() << ": " << red << "error: " << white << "undefined token " << red << token.lexeme() << white << std::endl;
}

void Debug::ParseError(const Token &token1, Token::Type token2)
{
    if (token2 != Token::Nul)
    {
        std::cout << curFile << ":" << token1.line() << ": " << red << "error: " << token1.lexeme() << white << " is not a " << red << TokenTypeMap[token2] << white << std::endl;
    }
    else
    {
        std::cout << curFile << ":" << token1.line() << ": " << red << "error: " << white << "missing token before " << red << token1.lexeme() << white << std::endl;;
    }
}

void Debug::Redeclare(const Token &token)
{
    std::cout << curFile << ":" << token.line() << ": " << red << "error: " << white << "redeclaration of " << red << token.lexeme() << white << std::endl;
}

void Debug::TypeError(const Token &token)
{
    std::cout << curFile << ":" << token.line() << ": " << red << "error: " << white << "data type error " << red << token.lexeme() << white << std::endl;
}

void Debug::VarUndefined(const Token &token)
{
    std::cout << curFile << ":" << token.line() << ": " << red << "error: " << white << "undefined variable " << red << token.lexeme() << white << std::endl;
}

void Debug::NotArray(const Token &token)
{
    std::cout << curFile << ":" << token.line() << ": " << red << "error: "  << red << token.lexeme() << white << " is not a array" << white << std::endl;
}

void Debug::NotFunction(const Token &token)
{
    std::cout << curFile << ":" << token.line() << ": " << red << "error: "  << red << token.lexeme() << white << " is not a function" << white << std::endl;
}

void Debug::NotNormalVar(const Token &token)
{
    std::cout << curFile << ":" << token.line() << ": " << red << "error: "  << red << token.lexeme() << white << " is not a normal variable" << white << std::endl;
}

void Debug::ParamsError(const Token &token, const std::string &str)
{
    std::cout << curFile << ":" << token.line() << ": " << red << "error: "  << white << "too " << str << " arguments to function " << red << token.lexeme() << white << std::endl;
}

void Debug::AssignToArrayType(const Token &token)
{
    std::cout << curFile << ":" << token.line() << ": " << red << "error: "  << white << "assignment to expression with array "  << red << token.lexeme() << white << std::endl;
}

void Debug::SetCurrentFile(const std::string &file)
{
    curFile = file;
}

void Debug::PrintTokens(const std::vector<Token> &tokens)
{
    std::cout << green << "Tokens:" << white << std::endl;
    for (auto token : tokens)
    {
        std::cout << "Token( " << TokenTypeMap[token.type()] << " '" << token.lexeme() << "' " << token.line() << " )" << std::endl;
    }
    std::cout << std::endl;
}

void Debug::PrintTAC(const std::vector<TACGenerator::Quaternion> &tac)
{
    std::cout << green << "TAC:" << white << std::endl;
    for (const auto &code : tac)
    {
        std::cout << std::endl;
        switch(code.op)
        {
            case TACGenerator::Op_global_var:
            std::cout << "global " << code.result << ", " << code.arg1 << std::endl;
            break;
            case TACGenerator::Op_local_var:
            std::cout << "local " << code.result << ", " << code.arg1 << std::endl;
            break;
            case TACGenerator::Op_begin_func:
            std::cout << "begin_function " << code.result << std::endl;
            break;
            case TACGenerator::Op_end_func:
            std::cout << "end_function " << code.result << std::endl;
            break;
            case TACGenerator::Op_dec_param:
            std::cout << "dec_param " << code.result << std::endl;
            break;
            case TACGenerator::Op_param:
            std::cout << "param " << code.result << std::endl;
            break;
            case TACGenerator::Op_label:
            std::cout << code.result << ": " << std::endl;
            break;
            case TACGenerator::Op_goto:
            std::cout << "goto " << code.result << std::endl;
            break;
            case TACGenerator::Op_if:
            std::cout << "if ( " << code.arg1 << ") goto " << code.result << std::endl;
            break;
            case TACGenerator::Op_assign:
            std::cout << code.result << " = " << code.arg1 << std::endl;
            break;
            case TACGenerator::Op_call_func:
            std::cout << "call " << code.result << ", " << code.arg1 << std::endl;
            break;
            case TACGenerator::Op_add:
            std::cout << code.result << " = " << code.arg1 << " + " << code.arg2 << std::endl;
            break;
            case TACGenerator::Op_sub:
            std::cout << code.result << " = " << code.arg1 << " - " << code.arg2 << std::endl;
            break;
            case TACGenerator::Op_mult:
            std::cout << code.result << " = " << code.arg1 << " * " << code.arg2 << std::endl;
            break;
            case TACGenerator::Op_div:
            std::cout << code.result << " = " << code.arg1 << " / " << code.arg2 << std::endl;
            break;
            case TACGenerator::Op_mod:
            std::cout << code.result << " = " << code.arg1 << " % " << code.arg2 << std::endl;
            break;
            case TACGenerator::Op_eq_eq:
            std::cout << code.result << " = " << code.arg1 << " == " << code.arg2 << std::endl;
            break;
            case TACGenerator::Op_not_eq:
            std::cout << code.result << " = " << code.arg1 << " != " << code.arg2 << std::endl;
            break;
            case TACGenerator::Op_greater:
            std::cout << code.result << " = " << code.arg1 << " > " << code.arg2 << std::endl;
            break;
            case TACGenerator::Op_greater_eq:
            std::cout << code.result << " = " << code.arg1 << " >= " << code.arg2 << std::endl;
            break;
            case TACGenerator::Op_less:
            std::cout << code.result << " = " << code.arg1 << " < " << code.arg2 << std::endl;
            break;
            case TACGenerator::Op_less_eq:
            std::cout << code.result << " = " << code.arg1 << " <= " << code.arg2 << std::endl;
            break;
        }
    }
}