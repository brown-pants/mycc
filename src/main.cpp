#include "debug/Debug.h"
#include "util/Util.h"
#include "pret/Preprocessor.h"
#include "lex/Lexer.h"
#include "parse/Parser.h"
#include "codegen/3ac/TACGenerator.h"
#include "codegen/3ac/TACOptimizer.h"
#include "codegen/as/ASGenerator.h"
#include "symtab/SymbolTable.h"
#include <stdlib.h>

enum State
{
    Normal,
    Library,    // -l
    Output      // -o
};

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        Debug::NoInput();
        return -1;
    }

    State state = Normal;
    std::string output = "a.out";
    std::string objStr = "";

    for (int i = 1; i < argc; i ++)
    {
        const std::string &file_name = argv[i];
        if (file_name == "-l")
        {
            state = Library;
            continue;
        }
        else if (file_name == "-o")
        {
            state = Output;
            continue;
        }

        if (state == Library)
        {
            objStr += file_name;
            objStr += " ";
            continue;
        }
        else if (state == Output)
        {
            output = file_name;
            state = Normal;
            continue;
        }

        const std::string &f_name = Util::FileNameReplaceExtension(file_name);
        
        SymbolTable::GetInstance().clear();

        Preprocessor preprocessor(file_name);
        std::string source = preprocessor.exec();

        if (preprocessor.hasError())
        {
            return -1;
        }

        Util::WriteFile(f_name + ".i", source);

        Lexer lexer(source);
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
        std::vector<TACGenerator::Quaternion> &tac = tacGenerator.exec();

        if (tacGenerator.hasError())
        {
            return -1;
        }

        TACOptimizer tacOptimizer;
        tacOptimizer.opt(tac);

        Debug::PrintTAC(tac);

        ASGenerator asGenerator(tac);
        std::string asc = asGenerator.exec();

        Util::WriteFile(f_name + ".s", asc);

        system(("as " + f_name + ".s -o " + f_name + ".o").c_str());

        objStr += f_name + ".o ";
    }
    system(("ld " + objStr + "-o " + output).c_str());
    return 0;
}