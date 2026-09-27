#include "mesh_component.h"

#include "assets/types/shader_asset.h"
#include "models/mesh_factory.h"

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
        m_primitive = MeshPrimitive::None;
        m_pendingMesh.reset();
        m_mesh.request(std::move(path));
    }

    void MeshComponent::load(MeshAsset mesh)
    {
        m_primitive = MeshPrimitive::None;
        m_mesh.reset();
        m_pendingMesh = std::move(mesh);
    }

    void MeshComponent::loadPrimitive(MeshPrimitive primitive)
    {
        switch (primitive)
        {
            case MeshPrimitive::Cube:
                load(mesh::factory::generateCube());
                break;

            case MeshPrimitive::Pyramid:
                load(mesh::factory::generatePyramid());
                break;

            case MeshPrimitive::Sphere:
                load(mesh::factory::generateMeshSphere(32, 32));
                break;

            case MeshPrimitive::Cylinder:
                load(mesh::factory::generateMeshCylinder(32));
                break;

            case MeshPrimitive::Plane:
                load(mesh::factory::generateMeshPlane(1.0f, 1.0f, 2, 2));
                break;

            case MeshPrimitive::Torus:
                load(mesh::factory::generateMeshTorus());
                break;

            case MeshPrimitive::Knot:
                load(mesh::factory::generateMeshKnot());
                break;

            case MeshPrimitive::Quad:
                load(mesh::factory::generateQuad());
                break;

            case MeshPrimitive::Triangle:
                load(mesh::factory::generateTriangle());
                break;

            case MeshPrimitive::Circle:
                load(mesh::factory::generateCircle(32));
                break;

            case MeshPrimitive::None:
                return;
        }
        m_primitive = primitive;
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

            m_material.m_shaderId = context.load<ShaderAsset>("model");

            context.loadAsset<ShaderAsset>(m_material.m_shaderId);
        }

        if (m_pendingMesh.has_value())
        {
            const AssetId id = context.create(*m_pendingMesh);

            m_mesh.set(id);

            m_material.m_shaderId = context.load<ShaderAsset>("model");

            context.loadAsset<ShaderAsset>(m_material.m_shaderId);

            m_pendingMesh.reset();
        }

    }

}
