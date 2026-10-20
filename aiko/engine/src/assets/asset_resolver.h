#pragma once

#include "asset_configuration.h"

#include <filesystem>
#include <string>
#include <string_view>

namespace aiko
{
    enum class AssetOwner
    {
        Project,
        Engine
    };

    enum class AssetResolveError
    {
        None,
        InvalidPath,
        InvalidConfiguration,
        NotFound,
        FilesystemError
    };

    struct ResolvedAsset
    {
        AssetOwner owner;
        std::string logicalPath;
        std::string identity;
        std::filesystem::path physicalPath;
    };

    struct AssetResolveResult
    {
        AssetResolveError error = AssetResolveError::None;
        std::string message;
        ResolvedAsset asset{};

        explicit operator bool() const noexcept
        {
            return error == AssetResolveError::None;
        }
    };

    class AssetResolver final
    {
    public:
        explicit AssetResolver(AssetConfiguration configuration);

        [[nodiscard]]
        AssetResolveResult resolve(std::string_view source) const;

    private:
        AssetConfiguration m_configuration;
    };
}
