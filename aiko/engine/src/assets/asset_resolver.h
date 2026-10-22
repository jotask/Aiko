#pragma once

#include "asset_configuration.h"
#include "asset_resolver_types.h"

#include <filesystem>
#include <string>
#include <string_view>

namespace aiko
{
    class IResourceResolver;

    class AssetResolver final
    {
    public:
        AssetResolver(const IResourceResolver& resources, const AssetConfiguration& configuration);

        [[nodiscard]]
        AssetResolveResult resolve(std::string_view source) const;

        [[nodiscard]]
        AssetResolveResult resolveShaderBinary(std::string_view source) const;

        [[nodiscard]]
        ShaderPairResolveResult resolveShaderPair(std::string_view vertexSource, std::string_view fragmentSource) const;

    private:
        const IResourceResolver& m_resources;

        bool m_hasProjectAssets = false;
        bool m_hasProjectShaders = false;
    };
}