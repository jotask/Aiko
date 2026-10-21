#pragma once

#include "iresource_resolver.h"

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace aiko
{
    class FilesystemResourceResolver final : public IResourceResolver
    {
    public:
        [[nodiscard]]
        bool mount(std::string_view namespaceName, const std::filesystem::path& root, bool searchable = false);

        [[nodiscard]]
        ResourceResult resolve(std::string_view reference) const override;

    private:
        struct Mount
        {
            std::string name;
            std::filesystem::path root;
            bool searchable = false;
        };

        std::vector<Mount> m_mounts;
    };
}