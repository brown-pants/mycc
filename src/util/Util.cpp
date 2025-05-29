#include "Util.h"

#include <fstream>
#include <sstream>
#include <iostream>

std::string Util::ReadFile(const std::string &fileName)
{
    std::ifstream ifs(fileName);
    if (!ifs.is_open())
    {
        std::cout << "read file error: " << fileName << std::endl;
        exit(-1);
    }
    std::ostringstream oss;
    oss << ifs.rdbuf();
    return oss.str();
}

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