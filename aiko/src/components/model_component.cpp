#include "model_component.h"

#include "models/game_object.h"
#include "systems/render_system.h"
#include "systems/asset_system.h"
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
    }

    void ModelComponent::load(const char* filename)
    {
        m_model.load(filename);
    }

    void ModelComponent::update()
    {
        aiko::AikoPtr<Transform> transform = gameobject->transform();
    }

    void ModelComponent::render()
    {
        m_renderSystem->render(gameobject->transform().get(), &m_model);
    }

}
