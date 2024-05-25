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
        m_mesh = m_renderSystem->createMesh(Mesh::MeshType::TEST);
        m_shader = m_renderSystem->createShader(global::getAssetPath("shaders/aiko.vs"), global::getAssetPath("shaders/aiko.fs"));
    }

    void MeshComponent::update()
    {
        aiko::AikoPtr<Transform> transform = gameobject->transform();
    }

    void MeshComponent::render()
    {
        m_renderSystem->render( gameobject->transform().get(), m_mesh.get(), m_shader.get());
    }

}
