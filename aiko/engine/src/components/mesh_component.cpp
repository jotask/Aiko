#include "mesh_component.h"

#include "constants.h"
#include "models/game_object.h"
#include "systems/render_system.h"
#include "models/mesh_factory.h"

namespace aiko
{

    MeshComponent::MeshComponent()
        : Component("Mesh")
    {

    }

    void MeshComponent::init()
    {
        m_renderSystem = gameobject->getSystem<RenderSystem>();
        m_material.m_shader.load("model.vs", "model.fs");
        AIKO_ASSERT(m_material.m_shader.isValid(), "Shader is invalid");
        const Mesh::MeshData meshData = mesh::factory::generateCube();
        m_mesh.setData(meshData);
        AIKO_ASSERT(m_mesh.isValid(), "Mesh is invalid");
    }

    void MeshComponent::load(const char* filename)
    {
        m_mesh.load(filename);
        m_mesh.refresh();
    }

    void MeshComponent::refresh()
    {
        m_mesh.refresh();
    }

}
