#include "mesh_component.h"

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
    }

    void MeshComponent::loadDebugCube()
    {
        m_primitiveRequest = PrimitiveRequest::Cube;
    }

}
