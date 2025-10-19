#include "Preprocessor.h"
#include "../debug/Debug.h"

Preprocessor::Preprocessor(const std::string &file_name)
    : file_name(file_name), m_hasError(false), lineHead(true), preCh('\0')
{
}

std::string Preprocessor::exec()
{
    FileInfo file_info = openFile(file_name);
    if (m_hasError)
    {
        Debug::OpenFileError(file_name, file_name);
        return "";
    }
    file_stack.push(file_info);
    output += "# 1 \"" + file_name + "\"\n";

    while(!file_stack.empty() && !m_hasError)
    {
        FileInfo &file_info = file_stack.top();
        char ch = file_info.fs->get();
        if (ch == EOF)
        {
            preCh = '\0';
            file_stack.pop();
            if (file_stack.empty())
            {
                break;
            }
            FileInfo &top_file = file_stack.top();
            output += "\n# " + std::to_string(top_file.curLine) + " \"" + top_file.file_name + "\"\n";
        }
        else if (ch == '\n')
        {
            preCh = ch;
            lineHead = true;
            output += ch;
            file_info.curLine ++;
        }
        else if (ch == '#')
        {
            preCh = ch;
            if (!lineHead)
            {
                output += ch;
                continue;
            }
            std::string line;
            std::getline(*file_info.fs, line);
            handle_include(line + '\n', file_info);
            file_info.curLine ++;
        }
        else if (ch == '/')
        {
            if (preCh == '/')
            {
                std::string line;
                std::getline(*file_info.fs, line);
                file_info.curLine ++;
                output.erase(output.length() - 1);
                output += '\n';
                lineHead = true;
                preCh = '\0';
                continue;
            }
            preCh = ch;
            lineHead = false;
            output += ch;
        }
        else if (ch == '*')
        {
            if (preCh == '/')
            {
                output.erase(output.length() - 1);
                remove_comment(file_info);
                continue;
            }
            preCh = ch;
            lineHead = false;
            output += ch;
        }
        else if (ch == ' ' || ch == '\t')
        {
            preCh = ch;
            output += ch;
        }
        else
        {
            preCh = ch;
            lineHead = false;
            output += ch;
        }
    }
    return output;
}

bool Preprocessor::hasError() const
{
    return m_hasError;
}

Preprocessor::FileInfo Preprocessor::openFile(const std::string &file_name)
{
    FileInfo file_info;
    fs_ptr fs = std::make_shared<std::ifstream>(file_name);
    if (!fs->is_open())
    {
        m_hasError = true;
        return file_info;
    }
    file_info.fs = fs;
    file_info.file_name = file_name;
    file_info.curLine = 1;
    return file_info;
}

void Preprocessor::handle_include(const std::string &line, const FileInfo &file_info)
{
    int pos = 0;
    std::string instru = "";

    // skip white
    while (line[pos] == ' ' || line[pos] == '\t')
    {
        pos ++;
    }

    // get instruction
    while(std::isalpha(line[pos]))
    {
        instru += line[pos ++];
    }

    if (instru != "include")
    {
        Debug::InvalidPreprocessing(file_info.file_name, file_info.curLine, instru);
        m_hasError = true;
        return;
    }
    
    // skip white
    while (line[pos] == ' ' || line[pos] == '\t')
    {
        pos ++;
    }

    if (line[pos ++] != '\"')
    {
        Debug::IncludeExpects(file_info.file_name, file_info.curLine);
        m_hasError = true;
        return;
    }
    
    std::string include_file;

    while (line[pos] != '\"' && line[pos] != '\n')
    {
        include_file += line[pos ++];
    }

    if (line[pos] != '\"')
    {
        Debug::IncludeExpects(file_info.file_name, file_info.curLine);
        m_hasError = true;
        return;
    }

    FileInfo info = openFile(include_file);
    if (m_hasError)
    {
        Debug::OpenFileError(file_info.file_name + ":" + std::to_string(file_info.curLine), include_file);
    }
    file_stack.push(info);

    // erase white
    size_t find_pos = output.find_last_not_of(" \t");
    if (find_pos != std::string::npos)
    {
        output.erase(find_pos + 1);
    } 
    else 
    {
        output.clear();
    }
    output += "# 1 \"" + include_file + "\"\n";
}

void Preprocessor::remove_comment(FileInfo &file_info)
{
    /*comment
    ...*/
    char ch = file_info.fs->get();
    while (ch != EOF)
    {
        if (ch == '\n')
        {
            output += ch;
            file_info.curLine ++;
        }
        else if (ch == '*')
        {
            ch = file_info.fs->get();
            if (ch == '/')
            {
                preCh = '\0';
                break;
            }
        }
        ch = file_info.fs->get();
    }
}