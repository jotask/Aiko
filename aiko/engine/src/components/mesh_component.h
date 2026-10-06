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

        enum class MeshPrimitive
        {
            None,
            Cube,
            Pyramid,
            Sphere,
            Cylinder,
            Plane,
            Torus,
            Knot,
            Quad,
            Triangle,
            Circle
        };

        MeshComponent();
        virtual ~MeshComponent() = default;

        void load(string path);
        void load(MeshAsset mesh);
        void load(const AssetId& id);

        void loadPrimitive(MeshPrimitive primitive);
        MeshPrimitive getPrimitive() const { return m_primitive; }

        const AssetId& getMeshId() const;
        const string& getAssetSource() const { return m_mesh.source(); }

        Material& getMaterial() { return m_material; }
        const Material& getMaterial() const { return m_material; }
    protected:
        virtual void init() override;
    private:

        void resolveAssetBinding(AssetBindingContext& context) override;

        MeshPrimitive m_primitive = MeshPrimitive::None;

        AssetReference<MeshAsset> m_mesh;
        Material m_material;
        std::optional<MeshAsset> m_pendingMesh;

    };

}
