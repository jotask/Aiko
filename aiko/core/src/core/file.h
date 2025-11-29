#pragme once

#include "aiko_types.h"

#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>

namespace aiko
{
    namespace files
    {
        string readFileContent(const char* file_path)
        {
            AIKO_ASSERT(std::filesystem::exists(file_path), "File don't exist!");
            std::ifstream file(file_path);
            AIKO_ASSERT(file.fail() == false, "Failed to open and read file");
            std::stringstream shaderStream;
            shaderStream << file.rdbuf();
            file.close();
            return string(shaderStream.str());
        }

        std::vector<uint8_t> readFileBytes(const char* file_path)
        {
            AIKO_ASSERT(std::filesystem::exists(file_path), "File don't exist!");
            std::ifstream file(file_path, std::ios::binary | std::ios::ate);
            AIKO_ASSERT(file.fail() == false, "Failed to open and read file");
            return std::vector<uint8_t>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        }

    }
}