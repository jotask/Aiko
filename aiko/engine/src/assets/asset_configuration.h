#pragma once

#include <filesystem>

namespace aiko
{

    struct AssetConfiguration
    {
        // Empty means no project asset root was configured.
        std::filesystem::path projectRoot;

        // Engine-owned assets, resolved during initialization.
        std::filesystem::path engineRoot;

        // Compiled shader roots, not shader source directories.
        std::filesystem::path projectShaderBinaryRoot;
        std::filesystem::path engineShaderBinaryRoot;
    };

}