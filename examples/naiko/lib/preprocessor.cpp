#include "preprocessor.h"

#include <core/file.h>

#include <regex>
#include <filesystem>

#include <logger/logger.h>

namespace aiko::naiko
{
    Preprocessor::Preprocessor(std::vector<string> files)
        : m_files(files)
    {

    }

    string Preprocessor::parse()
    {
        string final;
        for (auto file : m_files)
        {
            string content = processFile(file);
            final.append(content);
        }
        std::filesystem::path front = std::filesystem::path(m_files.front());
        std::filesystem::path path = std::filesystem::absolute(front);
        std::ofstream out(path.parent_path() / "out.naiko", std::ios::binary);
        out << final;
        out.close();
        return final;
    }

    string Preprocessor::processFile(string file)
    {

        auto trim = [](const string& str) -> string
        {
            string s(str);
            s.erase(0,s.find_first_not_of(" \n\r\t"));
            s.erase(s.find_last_not_of(" \n\r\t")+1);
            return s;
        };

        if (m_processedFiles.contains(file) == true)
        {
            return "";
        }
        m_processedFiles.insert(file);

        auto content = files::readFileContent(file.c_str());
        std::istringstream lines(content);
        string final;
        string line;

        std::filesystem::path currentFilePath = file;

        while (getline(lines, line))
        {
            auto trimmed = trim(line);
            if (trimmed.starts_with("#") == false)
            {
                final.append(line + "\n"); // keep normal lines
                continue;
            }

            if (trimmed.starts_with("#include") == true)
            {
                std::regex rex(R"(#include\s*\"([^\"]+)\")");
                std::smatch match;
                if (std::regex_search(trimmed, match, rex) == true)
                {
                    string includePath = match[1].str() + ".naiko" ;
                    std::filesystem::path includeFullPath = currentFilePath.parent_path() / includePath;
                    final.append(processFile(includeFullPath.string()));
                }
                else
                {
                    logger::Log::error("Invalid #include syntax: %s", trimmed.c_str());
                }
            }

        }

        return  final;
    }
}
