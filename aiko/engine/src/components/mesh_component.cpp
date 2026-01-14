#include "mesh_component.h"

#include "models/game_object.h"
#include "systems/render_system.h"
#include "systems/asset_system.h"
#include "constants.h"
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
        m_shader.load("cubes.vs", "cubes.fs");
        AIKO_ASSERT(m_shader.isValid(), "Shader is invalid");
        const Mesh::MeshData meshData = mesh::factory::generateCube();
        m_mesh.setData(meshData);
        AIKO_ASSERT(m_mesh.isValid(), "Mesh is invalid");
    }

    void MeshComponent::update()
    {

    }

    void MeshComponent::render()
    {
        m_renderSystem->render(gameobject->transform(), m_mesh, m_shader);
    }

    void MeshComponent::load(const char* filename)
    {
        auto ass = gameobject->getSystem<AssetSystem>();
        m_mesh = ass->loadMesh(filename);
        m_mesh.refresh();
    }

    void MeshComponent::refresh()
    {
        AIKO_DEBUG_BREAK
    }

}
