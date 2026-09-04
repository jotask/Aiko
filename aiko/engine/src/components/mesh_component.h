#pragma once

#include "models/component.h"
#include "assets/asset_id.h"
#include "models/material.h"
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

        Material& getMaterial() { return m_material; }
        const Material& getMaterial() const { return m_material; }
    protected:
        virtual void init() override;
    private:

        void resolveAssetBinding(AssetBindingContext& context) override;

        AssetReference<MeshAsset> m_mesh;
        Material m_material;
        std::optional<MeshAsset> m_pendingMesh;

    };

}
