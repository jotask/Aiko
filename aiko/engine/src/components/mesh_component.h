#pragma once

#include "models/component.h"
#include "assets/asset_id.h"
#include "metadata/material_instance.h"
#include "assets/asset_reference.h"
#include "assets/types/mesh_asset.h"
#include "assets/asset_binding.h"

namespace aiko
{

    class MeshComponent : public Component, public IAssetBinding
    {
    public:

        MeshComponent();
        virtual ~MeshComponent() = default;

        virtual void init() override;

        void load(string path);
        void loadDebugCube();

        const AssetId& getMeshId() const { return m_mesh.id(); }

        MaterialAsset& getMaterial() { return m_material; }
        const MaterialAsset& getMaterial() const { return m_material; }

        MaterialInstance& getMaterialInstance() { return m_materialInstance; }
        const MaterialInstance& getMaterialInstance() const { return m_materialInstance; }

    private:

        enum class PrimitiveRequest
        {
            None,
            Cube
        };

        void resolveAssetBinding(AssetBindingContext& context) override;

        AssetReference<MeshAsset> m_mesh;
        PrimitiveRequest m_primitiveRequest = PrimitiveRequest::None;

        MaterialAsset   m_material;
        MaterialInstance m_materialInstance;

    };

}
