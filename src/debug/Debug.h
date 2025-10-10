#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "../token/Token.h"
#include "../codegen/3ac/TACGenerator.h"
#include <unordered_map>
#include <vector>

class Debug
{
public:
    static void NoInput();

    static void LexicalError(const Token &token);
    static void ParseError(const Token &token1, Token::Type token2 = Token::Nul);
    static void Redeclare(const Token &token);
    static void TypeError(const Token &token);
    static void VarUndefined(const Token &token);
    static void ArrayUndefined(const Token &token);
    static void NotArray(const Token &token);
    static void NotFunction(const Token &token);
    static void NotNormalVar(const Token &token);
    static void ParamsError(const Token &token, const std::string &str);
    static void AssignToErrorType(const Token &token, const std::string &type);
    static void NotSupportedPointerArray(const Token &token);
    static void DereferencingError(const Token &token, const std::string &type);
    static void InvalidOperands(const Token &token);
    static void InitialNotConstant(const Token &token);
    static void NotWithinLoop(const Token &token);

    static void SetCurrentFile(const std::string &file);

    static void PrintTokens(const std::vector<Token> &tokens);
    static void PrintTAC(const std::vector<TACGenerator::Quaternion> &tac);

private:
    Debug();
    static std::string curFile;
    static std::unordered_map<Token::Type, std::string> TokenTypeMap;
};

#endif