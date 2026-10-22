#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace aiko
{
    enum class AssetOwner
    {
        Project,
        Engine,
        Renderer
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

    struct ShaderPairResolveResult
    {
        AssetResolveError error = AssetResolveError::None;
        std::string message;

        ResolvedAsset vertex;
        ResolvedAsset fragment;

        explicit operator bool() const noexcept
        {
            return error == AssetResolveError::None;
        }
    };
}
