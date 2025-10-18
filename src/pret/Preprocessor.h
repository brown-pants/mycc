#ifndef __PREPROCESSOR_H__
#define __PREPROCESSOR_H__

#include <string>
#include <stack>
#include <fstream>
#include <memory>

class Preprocessor
{
public:
    Preprocessor(const std::string &file_name);
    std::string exec();
    bool hasError() const;

private:
    using fs_ptr = std::shared_ptr<std::ifstream>;
    struct FileInfo
    {
        fs_ptr fs;
        std::string file_name;
        int curLine;
    };
    std::stack<FileInfo> file_stack;
    const std::string &file_name;
    std::string output;
    bool m_hasError;
    bool lineHead;
    char preCh;
    FileInfo openFile(const std::string &file_name);
    void handle_include(const std::string &line, const FileInfo &file_info);
    void remove_comment(FileInfo &file_info);
};

#endif