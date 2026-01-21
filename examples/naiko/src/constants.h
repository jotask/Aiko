#pragma once

#include <string>

namespace naiko
{
    namespace global
    {
        inline constexpr const char* NAIKO_ASSET_PATH = "/home/jose/Projects/examples/naiko/src/";

        inline std::string getAssetPath(const char* subpath)
        {
            std::string buffer;
            buffer.clear();
            buffer += NAIKO_ASSET_PATH;
            buffer += subpath;
            return buffer;

        }

    }
}
