#include "debug/Debug.h"
#include "util/Util.h"
#include "lex/Lexer.h"
#include "parse/Parser.h"
#include "codegen/3ac/TACGenerator.h"
#include "codegen/as/ASGenerator.h"
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        Debug::NoInput();
        return -1;
    }

    const std::string &file_name = argv[1];
    Debug::SetCurrentFile(file_name);

    Lexer lexer(Util::ReadFile(file_name));
    std::vector<Token> tokens = lexer.exec();

    if (lexer.hasError())
    {
        return -1;
    }

    Debug::PrintTokens(tokens);

    Parser parser(tokens);
    Parser::TreeNode astRoot = parser.exec();
    
    if (parser.hasError())
    {
        return -1;
    }

    TACGenerator tacGenerator(astRoot);
    const std::vector<TACGenerator::Quaternion> &tac = tacGenerator.exec();

    if (tacGenerator.hasError())
    {
        return -1;
    }

    Debug::PrintTAC(tac);

    ASGenerator asGenerator(tac);
    std::string asc = asGenerator.exec();

    const std::string &f_name = Util::FileNameReplaceExtension(file_name);
    Util::WriteFile(f_name + ".s", asc);

    system(("as " + f_name + ".s -o " + f_name + ".o").c_str());

    system(("ld " + f_name + ".o -o " + f_name + ".out").c_str());

    return 0;
}