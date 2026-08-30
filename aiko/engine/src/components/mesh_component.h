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

        void setMeshId(const AssetId& id){ m_mesh.set(id); }
        const AssetId& getMeshId() const { return m_mesh.id(); }

        AssetReference<MeshAsset>& meshReference() { return m_mesh; }
        const AssetReference<MeshAsset>& meshReference() const { return m_mesh; }

        bool hasCubeRequest() const { return m_primitiveRequest == PrimitiveRequest::Cube; }
        void clearPrimitiveRequest() { m_primitiveRequest = PrimitiveRequest::None; }

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
