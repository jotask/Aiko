#include "model_component.h"

#include "models/game_object.h"
#include "systems/render_system.h"
#include "constants.h"

namespace aiko
{

    ModelComponent::ModelComponent()
        : Component("Model")
    {

    }

    void ModelComponent::init()
    {
        m_renderSystem = gameobject->getSystem<RenderSystem>();
        m_model.m_mesh = m_renderSystem->createMesh(Mesh::MeshType::CUBE);
        m_model.m_material.m_shader.load("aiko.vs", "shaders/aiko.fs");
        assert(m_model.m_material.m_shader.isvalid() && "Shader is invalid");
        AIKO_DEBUG_BREAK
    }

    void ModelComponent::update()
    {
        aiko::AikoPtr<Transform> transform = gameobject->transform();
    }

    void ModelComponent::render()
    {
        m_renderSystem->render(gameobject->transform().get(), &m_model.m_mesh, &m_model.m_material.m_shader);
    }

}
