#include "grid_component.h"

#include "models/game_object.h"
#include "systems/render_system.h"

namespace aiko
{

    GridXComponent::GridXComponent()
        : Component("GridX")
    {

    }

    void GridXComponent::init()
    {
        m_renderSystem = gameobject->getSystem<RenderSystem>();
    }

    void GridXComponent::update()
    {
        // aiko::AikoPtr<Transform> transform = gameobject->transform();
    }

    void GridXComponent::render()
    {
        // m_renderSystem->render(this);
    }

}
