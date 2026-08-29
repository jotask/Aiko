#pragma once

#include <filesystem>

namespace aiko
{

    class AssetRegistry;

    class AssetRegistrySerializer
    {
    public:

        static bool save(const AssetRegistry& registry, const std::filesystem::path& path);
        static bool load(AssetRegistry& registry, const std::filesystem::path& path);

    };

}
