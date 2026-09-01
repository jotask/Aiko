#include "layers/layer_context.h"

#include "systems/input_system.h"
#include "systems/scene_system.h"
#include "systems/system_connector.h"

#include <time/time.h>

#include <utility>

namespace aiko
{
    LayerContext::LayerContext(SystemConnector& connector)
    {
        m_inputSystem = connector.find<InputSystem>();
        AIKO_ASSERT(m_inputSystem != nullptr, "Required system InputSystem not found");

        m_sceneSystem = connector.find<SceneSystem>();
        AIKO_ASSERT(m_sceneSystem != nullptr, "Required system SceneSystem not found");

    }

    GameObject* LayerContext::Instantiate(string name)
    {
        return m_sceneSystem->createGameObject(std::move(name)).get();
    }

    GameObject* LayerContext::Instantiate(GameObject* parent, string name)
    {
        return m_sceneSystem->createGameObject(parent, std::move(name)).get();
    }

    bool LayerContext::isKeyPressed(Key key) const
    {
        return m_inputSystem->isKeyPressed(key);
    }

    bool LayerContext::isKeyJustPressed(Key key) const
    {
        return m_inputSystem->isKeyJustPressed(key);
    }

    vec2 LayerContext::getMousePosition() const
    {
        return m_inputSystem->getMousePosition();
    }

    bool LayerContext::isMouseButtonPressed(MouseButton button) const
    {
        return m_inputSystem->isMouseButtonPressed(button);
    }

    float LayerContext::getDeltaTime() const
    {
        return Time::it().getDeltaTime();
    }
}
