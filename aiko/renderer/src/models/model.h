#pragma once

#include <vector>

#include "assets/types/mesh_asset.h"
#include "assets/iasset_provider.h"

namespace aiko
{

    class RenderResourceManager;

    class Model
    {
    public:

        struct SubMesh
        {
            AssetId meshId = InvalidAssetId;
            const MaterialAsset* material = nullptr;
        };

        // Copy
        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        // Move
        Model(Model&&) noexcept = default;
        Model& operator=(Model&&) noexcept = default;

        Model() = default;
        ~Model() = default;

        void upload(const ModelAsset& asset);
        void unload();

        const std::vector<SubMesh>& getSubMeshes() const { return m_subMeshes; }
        std::vector<SubMesh>& getSubMeshes() { return m_subMeshes; }

    private:

        std::vector<SubMesh> m_subMeshes;

    };
}
