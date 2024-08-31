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
    }

    void ModelComponent::update()
    {
        aiko::AikoPtr<Transform> transform = gameobject->transform();
    }

    void ModelComponent::render()
    {

    }

}
