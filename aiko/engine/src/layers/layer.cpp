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
    InputContext& Layer::input()
    {
        AIKO_ASSERT(m_context != nullptr, "Layer context not connected");
        return m_context->input();
    }

    const InputContext& Layer::input() const
    {
        AIKO_ASSERT(m_context != nullptr, "Layer context not connected");
        return m_context->input();

    }

}
