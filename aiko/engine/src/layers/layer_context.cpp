#include "layers/layer_context.h"

#include "systems/input_system.h"
#include "systems/scene_system.h"
#include "systems/system_connector.h"
#include "systems/render_system.h"

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

        m_renderSystem = connector.find<RenderSystem>();
        AIKO_ASSERT(m_renderSystem != nullptr, "Required system RenderSystem not found");

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

    vec2 LayerContext::getMouseDelta() const
    {
        return m_inputSystem->getMouseDelta();
    }

    void LayerContext::setIsMouseCentred(bool centred) const
    {
        m_inputSystem->setIsMouseCentred(centred);
    }

    float LayerContext::getDeltaTime() const
    {
        return Time::it().getDeltaTime();
    }

    void LayerContext::drawRectangle(const vec3& position, const vec3& size)
    {
        m_renderSystem->renderRectangle(position, size);
    }

}
