#include "Util.h"

#include <fstream>
#include <sstream>
#include <iostream>

void Util::WriteFile(const std::string &fileName, const std::string &str)
{
    std::ofstream outFile(fileName);
 
    if (!outFile.is_open()) {
        std::cerr << "write file error: " << fileName << std::endl;
        return;
    }
 
    outFile << str << std::endl;
 
    outFile.close();
}

std::string Util::FileNameReplaceExtension(const std::string &fileName)
{
    std::filesystem::path p(fileName);
    p.replace_extension();
    return p.string();
}