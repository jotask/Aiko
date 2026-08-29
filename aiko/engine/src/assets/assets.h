#pragma once

#include <type_traits>

#include "assets/asset_id.h"
#include "assets/asset_type.h"
#include "assets/types/mesh_asset.h"
#include "assets/types/shader_asset.h"
#include "assets/types/texture_asset.h"

namespace aiko
{

    class AssetManager;

    class Assets
    {
    public:

        explicit Assets(AssetManager& manager);

        template<typename T>
        AssetId load(string_view source)
        {
            return load(assetType<T>(), source);
        }

        AssetId create(const TextureAsset& asset);
        AssetId create(const MeshAsset& asset);

    private:

        template<typename>
        inline static constexpr bool ALWAYS_FALSE = false;

        template<typename T>
        static consteval AssetType assetType()
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
                static_assert(ALWAYS_FALSE<T>, "Unsupported asset type");
            }
        }

        AssetId load(AssetType type, string_view source);

        AssetManager& m_manager;

    };

}
