#include "mesh_component.h"

#include "models/game_object.h"
#include "systems/render_system.h"

namespace aiko
{

    MeshComponent::MeshComponent()
        : Component("Mesh")
    {

    }

    void MeshComponent::init()
    {
        m_renderSystem = gameobject->getSystem<RenderSystem>();
        m_mesh = m_renderSystem->createMesh();
        m_shader = m_renderSystem->createShader("C:\\Users\\j.iznardo\\Documents\\Aiko\\assets\\shaders\\aiko.vs", "C:\\Users\\j.iznardo\\Documents\\Aiko\\assets\\shaders\\aiko.fs");

    }

    void MeshComponent::update()
    {
        aiko::AikoPtr<Transform> transform = gameobject->transform();
    }

    void MeshComponent::render()
    {
        m_renderSystem->render(this);
    }

}
