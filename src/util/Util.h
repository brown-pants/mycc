#ifndef __UTIL_H__
#define __UTIL_H__

#include <string>
#include <filesystem>

class Util
{
public:
    static std::string ReadFile(const std::string &fileName);
    static void WriteFile(const std::string &fileName, const std::string &str);

    static std::string FileNameReplaceExtension(const std::string &fileName);

private:
    Util();
};

#endif