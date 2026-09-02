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
        return m_context->scene().Instantiate(std::move(name));
    }

    GameObject* Layer::Instantiate(GameObject* parent, string name)
    {
        AIKO_ASSERT(m_context != nullptr, "Layer context not connected");
        return m_context->scene().Instantiate(parent, std::move(name));
    }

    float Layer::getDeltaTime() const
    {
        AIKO_ASSERT(m_context != nullptr, "Layer context not connected");
        return m_context->getDeltaTime();
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

    RenderContext& Layer::renderer()
    {
        AIKO_ASSERT(m_context != nullptr, "Layer context not connected");
        return m_context->render();
    }

    const RenderContext& Layer::renderer() const
    {
        AIKO_ASSERT(m_context != nullptr, "Layer context not connected");
        return m_context->render();
    }

    SceneContext& Layer::scene()
    {
        AIKO_ASSERT(m_context != nullptr, "Layer context not connected");
        return m_context->scene();
    }

    const SceneContext& Layer::scene() const
    {
        AIKO_ASSERT(m_context != nullptr, "Layer context not connected");
        return m_context->scene();
    }

    AssetContext& Layer::assets()
    {
        AIKO_ASSERT(m_context != nullptr, "Layer context not connected");
        return m_context->assets();
    }

    const AssetContext& Layer::assets() const
    {
        AIKO_ASSERT(m_context != nullptr, "Layer context not connected");
        return m_context->assets();
    }

}
