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
        m_model.m_mesh = m_renderSystem->createMesh(Mesh::MeshType::TEST);
        m_model.m_material.m_shader = m_renderSystem->createShader(global::getAssetPath("shaders/aiko.vs").c_str(), global::getAssetPath("shaders/aiko.fs").c_str());
    }

    void ModelComponent::update()
    {
        aiko::AikoPtr<Transform> transform = gameobject->transform();
    }

    void ModelComponent::render()
    {
        m_renderSystem->render(gameobject->transform().get(), m_model.m_mesh.get(), m_model.m_material.m_shader.get());
    }

}
