#pragma once

#include "systems/base_system.h"

#include "assets/icomponent_assetaccess.h"
#include "resources/iresource_invalidator.h"

namespace aiko
{

    class RenderModule;
    class AssetsManagerModule;

    class AssetSystem : public BaseSystem, public IComponentAssetAccess, public IRenderResourceInvalidator
    {
    public:
        AssetSystem();
        virtual ~AssetSystem() = default;

        // IComponentAssetAccess

        virtual AssetId registerMesh(std::string_view path) override;
        virtual AssetId registerMesh(const MeshAsset& asset) override;
        virtual AssetId registerModel(std::string_view path) override;
        virtual AssetId registerTexture(std::string_view path) override;
        virtual AssetId registerTexture(const TextureAsset& asset) override;
        virtual AssetId registerComputeShader(std::string_view path) override;

        virtual TextureAsset& getMutableTextureAsset(const AssetId& id) override;

        // IRenderResourceInvalidator

        virtual void invalidateTexture(const AssetId& id) override;
        virtual void invalidateMesh(const AssetId& id) override;
        virtual void invalidateModel(const AssetId& id) override;
        virtual void invalidateShader(const AssetId& id) override;

    protected:

        virtual void connect(ModuleConnector*, SystemConnector*) override;

    private:

        RenderModule* m_renderModule;
        AssetsManagerModule* m_assetModule;

    };

}
