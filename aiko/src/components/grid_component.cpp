#include "grid_component.h"

#include "models/game_object.h"
#include "systems/render_system.h"

namespace aiko
{

    GridComponent::GridComponent()
        : Component("Grid")
    {

    }

    void GridComponent::init()
    {
        m_renderSystem = gameobject->getSystem<RenderSystem>();
    }

    void GridComponent::update()
    {
        // aiko::AikoPtr<Transform> transform = gameobject->transform();
    }

    void GridComponent::render()
    {
        // m_renderSystem->render(this);
    }

}
