#pragma once

#include "models/component.h"
#include "assets/asset_id.h"
#include "metadata/material_instance.h"
#include "assets/asset_reference.h"
#include "assets/types/mesh_asset.h"
#include "assets/asset_binding.h"

#include <optional>

namespace aiko
{

    class MeshComponent : public Component, public IAssetBinding
    {
    public:

        MeshComponent();
        virtual ~MeshComponent() = default;

        void load(string path);
        void load(MeshAsset mesh);

        const AssetId& getMeshId() const;

        MaterialAsset& getMaterial() { return m_material; }
        const MaterialAsset& getMaterial() const { return m_material; }

        MaterialInstance& getMaterialInstance() { return m_materialInstance; }
        const MaterialInstance& getMaterialInstance() const { return m_materialInstance; }
    protected:
        virtual void init() override;
    private:

        bool resolveAssetBinding(AssetBindingContext& context) override;

        AssetReference<MeshAsset> m_mesh;
        MaterialAsset   m_material;
        MaterialInstance m_materialInstance;
        std::optional<MeshAsset> m_pendingMesh;

    };

}
