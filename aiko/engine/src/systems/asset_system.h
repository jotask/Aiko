#pragma once

#include "systems/base_system.h"

#include "assets/asset_type.h"
#include "assets/types/shader_asset.h"

#include <type_traits>

namespace aiko
{

    class RenderModule;
    class AssetsManagerModule;

    class AssetSystem : public BaseSystem
    {
    public:
        AssetSystem();
        virtual ~AssetSystem() = default;

        template<typename T>
        AssetId load(string_view source)
        {
            return load(assetType<T>(), source);
        }

        AssetId create(const TextureAsset& asset);
        AssetId create(const MeshAsset& asset);

        TextureAsset& getMutableTextureAsset(const AssetId& id);

        void invalidateTexture(const AssetId& id);
        void invalidateMesh(const AssetId& id);
        void invalidateModel(const AssetId& id);
        void invalidateShader(const AssetId& id);

    protected:

        virtual void connect(ModuleConnector*, SystemConnector*) override;

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

        RenderModule* m_renderModule;
        AssetsManagerModule* m_assetModule;

    };

}
