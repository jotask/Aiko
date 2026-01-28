#include "model_component.h"

#include "constants.h"
#include "models/game_object.h"
#include "systems/render_system.h"

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

    }

    void ModelComponent::render()
    {
        m_renderSystem->render(gameobject->transform(), m_model);
    }

}
