#pragma once

#include <aiko_types.h>
#include <unordered_set>
#include <vector>

namespace aiko::naiko
{

    class Preprocessor
    {
    public:

        Preprocessor(std::vector<string>);

        string parse();

    private:

        string processFile(string);

        std::unordered_set<string> m_processedFiles;

        std::vector<string> m_files;
    };

}
