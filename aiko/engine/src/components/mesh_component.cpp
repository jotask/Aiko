#include "mesh_component.h"

#include "models/mesh_factory.h"
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
        m_mesh.request(std::move(path));
        m_primitiveRequest = PrimitiveRequest::None;
        markAssetBindingDirty();
    }

    void MeshComponent::loadDebugCube()
    {
        m_primitiveRequest = PrimitiveRequest::Cube;
        markAssetBindingDirty();
    }

    void MeshComponent::resolveAssetBinding(AssetBindingContext& context)
    {
        if (m_mesh.isRequested())
        {
            m_mesh.markLoading();

            const AssetId id = context.load<MeshAsset>(m_mesh.source());

            if (id == InvalidAssetId)
            {
                m_mesh.fail();
            }
            else
            {
                m_mesh.resolve(id);
            }
        }

        if (m_primitiveRequest == PrimitiveRequest::Cube)
        {
            const AssetId meshId = context.create(mesh::factory::generateCube());

            setMeshId(meshId);
            m_material.shaderId = context.load<ShaderAsset>("model");

            m_primitiveRequest = PrimitiveRequest::None;
        }
    }

}
