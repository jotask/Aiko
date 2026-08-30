#include "mesh_component.h"

#include "assets/types/shader_asset.h"

namespace aiko
{

    MeshComponent::MeshComponent()
        : Component("Mesh")
    {
    }

    void MeshComponent::init()
    {

    }

    void MeshComponent::load(string path)
    {
        m_pendingMesh.reset();
        m_mesh.request(std::move(path));
        markAssetBindingDirty();
    }

    void MeshComponent::load(MeshAsset mesh)
    {
        m_mesh.reset();
        m_pendingMesh = std::move(mesh);
        markAssetBindingDirty();
    }

    bool MeshComponent::resolveAssetBinding(AssetBindingContext& context)
    {
        if (m_mesh.isRequested())
        {
            const AssetId id = context.load<MeshAsset>(m_mesh.source());
            if (id == InvalidAssetId)
            {
                m_mesh.fail();
            }
            else
            {
                m_mesh.markLoading(id);
                m_mesh.resolve(id);
            }
        }

        if (m_pendingMesh.has_value())
        {
            const AssetId id = context.create(*m_pendingMesh);

            m_mesh.set(id);
            m_material.shaderId = context.load<ShaderAsset>("model");

            m_pendingMesh.reset();
        }
        return false;
    }

}
