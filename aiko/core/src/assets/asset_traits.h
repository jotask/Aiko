#pragma once

#include <type_traits>

#include "assets/asset_type.h"

namespace aiko
{
    struct TextureAsset;
    struct MeshAsset;
    struct ModelAsset;
    struct ShaderAsset;
    struct ComputeShaderAsset;

    template<typename>
    inline static constexpr bool ALWAYS_FALSE_ASSET_TYPE = false;

    template<typename T>
    consteval AssetType assetTypeOf()
    {
        if constexpr (std::is_same_v<T, TextureAsset>)
        {
            return AssetType::Texture;
        }
        else if constexpr (std::is_same_v<T, MeshAsset>)
        {
            return AssetType::Mesh;
        }
        else if constexpr (std::is_same_v<T, ModelAsset>)
        {
            return AssetType::Model;
        }
        else if constexpr (std::is_same_v<T, ShaderAsset>)
        {
            return AssetType::Shader;
        }
        else if constexpr (std::is_same_v<T, ComputeShaderAsset>)
        {
            return AssetType::ComputeShader;
        }
        else
        {
            static_assert(ALWAYS_FALSE_ASSET_TYPE<T>, "Unsupported asset type");
        }
    }
}
