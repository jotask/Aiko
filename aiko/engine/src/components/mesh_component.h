#pragma once

#include "models/component.h"
#include "assets/types/mesh_asset.h"
#include "assets/asset_id.h"
#include "metadata/material_instance.h"

namespace aiko
{

    class MeshComponent : public Component
    {
    public:

        MeshComponent();
        virtual ~MeshComponent() = default;

        virtual void init() override;

        void load(string path);
        void loadDebugCube();

        void setMeshId(const AssetId& id) { m_meshId = id; }
        const AssetId& getMeshId() const { return m_meshId; };
        MaterialAsset& getMaterial() { return m_material; }
        const MaterialAsset& getMaterial() const { return m_material; }

        MaterialInstance& getMaterialInstance() { return m_materialInstance; }
        const MaterialInstance& getMaterialInstance() const { return m_materialInstance; }

    private:

        AssetId         m_meshId = InvalidAssetId;
        MaterialAsset   m_material;
        MaterialInstance m_materialInstance;

    };

}
