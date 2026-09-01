#include "layers/layer.h"

#include "layers/layer_context.h"

#include <utility>

namespace aiko
{
    GameObject* Layer::Instantiate(string name)
    {
        AIKO_ASSERT(m_context != nullptr, "Layer context not connected");
        return m_context->Instantiate(std::move(name));
    }

    GameObject* Layer::Instantiate(GameObject* parent, string name)
    {
        AIKO_ASSERT(m_context != nullptr, "Layer context not connected");
        return m_context->Instantiate(parent, std::move(name));
    }

    bool Layer::isKeyPressed(Key key) const
    {
        AIKO_ASSERT(m_context != nullptr, "Layer context not connected");
        return m_context->isKeyPressed(key);
    }

    bool Layer::isKeyJustPressed(Key key) const
    {
        AIKO_ASSERT(m_context != nullptr, "Layer context not connected");
        return m_context->isKeyJustPressed(key);
    }

    vec2 Layer::getMousePosition() const
    {
        AIKO_ASSERT(m_context != nullptr, "Layer context not connected");
        return m_context->getMousePosition();
    }

    bool Layer::isMouseButtonPressed(MouseButton button) const
    {
        AIKO_ASSERT(m_context != nullptr, "Layer context not connected");
        return m_context->isMouseButtonPressed(button);
    }

    float Layer::getDeltaTime() const
    {
        AIKO_ASSERT(m_context != nullptr, "Layer context not connected");
        return m_context->getDeltaTime();
    }
}
