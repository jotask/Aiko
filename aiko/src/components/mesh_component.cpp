#include "mesh_component.h"

#include "models/game_object.h"
#include "systems/render_system.h"
#include "constants.h"

namespace aiko
{

    MeshComponent::MeshComponent()
        : Component("Mesh")
    {

    }

    void MeshComponent::init()
    {
        m_renderSystem = gameobject->getSystem<RenderSystem>();
        m_shader.load("vs_cubes", "fs_cubes");
        m_mesh = m_renderSystem->createMesh(Mesh::MeshType::TEST);
        assert(m_shader.isvalid() && "Shader is invalid");
    }

    void MeshComponent::update()
    {
        aiko::AikoPtr<Transform> transform = gameobject->transform();
    }

    void MeshComponent::render()
    {
        m_renderSystem->render( gameobject->transform().get(), &m_mesh, &m_shader);
    }

}
