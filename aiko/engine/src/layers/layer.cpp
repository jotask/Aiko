#include "layers/layer.h"

#include "layers/layer_context.h"

#include <utility>

namespace aiko
{

    LayerContext& Layer::context()
    {
        AIKO_ASSERT(m_context != nullptr, "Layer context not connected");
        return *m_context;
    }

    const LayerContext& Layer::context() const
    {
        AIKO_ASSERT(m_context != nullptr, "Layer context not connected");
        return *m_context;
    }

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
    vec2 Layer::getMouseDelta() const
    {
        AIKO_ASSERT(m_context != nullptr, "Layer context not connected");
        return m_context->getMouseDelta();
    }

    void Layer::setIsMouseCentred(bool centred) const
    {
        AIKO_ASSERT(m_context != nullptr, "Layer context not connected");
        m_context->setIsMouseCentred(centred);
    }

    float Layer::getDeltaTime() const
    {
        AIKO_ASSERT(m_context != nullptr, "Layer context not connected");
        return m_context->getDeltaTime();
    }

    void Layer::drawRectangle(const vec3& position, const vec3& size)
    {
        AIKO_ASSERT(m_context != nullptr, "Layer context not connected");
        m_context->drawRectangle(position, size);
    }

    void Layer::drawMesh(const Transform& transform, const Mesh& mesh, const Material& material)
    {
        AIKO_ASSERT(m_context != nullptr, "Layer context not connected");
        m_context->drawMesh(transform, mesh, material);
    }

}
