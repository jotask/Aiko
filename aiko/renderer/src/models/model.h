#pragma once

#include "assets/types/mesh_asset.h"
#include "assets/iasset_provider.h"
#include "models/material.h"

namespace aiko
{

    class RenderResourceManager;

    class Model
    {
    public:

        struct SubMesh
        {
            AssetId meshId = InvalidAssetId;
            Material material{};
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

        const vector<SubMesh>& getSubMeshes() const { return m_subMeshes; }
        vector<SubMesh>& getSubMeshes() { return m_subMeshes; }

    private:

        vector<SubMesh> m_subMeshes;

    };
}
