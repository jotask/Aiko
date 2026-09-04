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
    }

    void MeshComponent::load(MeshAsset mesh)
    {
        m_mesh.reset();
        m_pendingMesh = std::move(mesh);
    }

    const AssetId& MeshComponent::getMeshId() const
    {
        return m_mesh.isReady() ? m_mesh.id() : InvalidAssetId;
    }

    void MeshComponent::resolveAssetBinding(AssetBindingContext& context)
    {
        if (m_mesh.isRequested())
        {
            const AssetId id = context.load<MeshAsset>(m_mesh.source());

            if (id == InvalidAssetId)
            {
                m_mesh.fail();
                return;
            }

            m_mesh.markLoading(id);
            context.loadAsset<MeshAsset>(id);

            return;
        }

        if (m_mesh.isLoading())
        {
            const AssetId& id = m_mesh.id();

            if (context.isLoaded<MeshAsset>(id) == false)
            {
                return;
            }

            m_mesh.resolve(id);
        }

        if (m_pendingMesh.has_value())
        {
            const AssetId id = context.create(*m_pendingMesh);

            m_mesh.set(id);
            m_material.m_shaderId = context.load<ShaderAsset>("model");

            m_pendingMesh.reset();
        }

    }

}
