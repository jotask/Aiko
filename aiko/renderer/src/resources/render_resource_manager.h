#pragma once

#include <unordered_map>

#include <assets/asset_id.h>
#include <assets/iasset_provider.h>

#include "models/texture.h"
#include "models/mesh.h"
#include "models/model.h"

namespace aiko
{

    class RenderResourceManager
    {
    public:

        explicit RenderResourceManager(IAssetProvider& assets);

        Texture& getTexture(const AssetId& id);
        Mesh& getMesh(const AssetId& id);
        Model& getModel(const AssetId& id);

        bool hasTexture(const AssetId& id) const;
        bool hasMesh(const AssetId& id) const;
        bool hasModel(const AssetId& id) const;

        void unloadTexture(const AssetId& id);
        void unloadMesh(const AssetId& id);
        void unloadModel(const AssetId& id);
        void clear();

    private:

        IAssetProvider& m_assetProvider;
        std::unordered_map<AssetId, AikoUPtr<Texture>> m_textures;
        std::unordered_map<AssetId, AikoUPtr<Mesh>> m_meshes;
        std::unordered_map<AssetId, AikoUPtr<Model>> m_models;

    };

}
