#include "render_module.h"

#include "modules/module_connector.h"
#include "modules/display_module.h"
#include "events/events.hpp"

namespace aiko
{
    RenderModule::RenderModule(Aiko* aiko)
        : BaseModule(aiko)
        , m_displayModule(nullptr)
    {

    }

    void RenderModule::connect(ModuleConnector* moduleConnector)
    {
        BIND_MODULE_REQUIRED(DisplayModule, moduleConnector, m_displayModule)
    }

    void RenderModule::init()
    {
        const AikoConfig cfg = getAiko()->getConfig();
        background_color = cfg.background_color;
        EventSystem::it().bind<WindowResizeEvent>(this, &RenderModule::onWindowResize);
    }

    void RenderModule::postInit()
    {
        std::for_each(m_renderers.begin(), m_renderers.end(), [](auto& ctx) { ctx.second->init(); });
    }

    void RenderModule::beginFrame()
    {
        std::for_each(m_renderers.begin(), m_renderers.end(), [](auto& ctx) { ctx.second->beginFrame(); });
    }

    void RenderModule::endFrame()
    {
        std::for_each(m_renderers.begin(), m_renderers.end(), [](auto& ctx) { ctx.second->endFrame(); });
    }

    void RenderModule::dispose()
    {
        std::for_each(m_renderers.begin(), m_renderers.end(), [](auto& ctx) { ctx.second->dispose(); });
    }

    ivec2 RenderModule::getDisplaySize()
    {
        return m_displayModule->getCurrentDisplay().getDisplaySize();
    }

    RenderContext* RenderModule::getRenderer(ContextType ctx)
    {
        return m_renderers[ctx].get();
    }

    void RenderModule::onWindowResize(Event& event)
    {
        const auto& msg = static_cast<const WindowResizeEvent&>(event);
        m_displayModule->getCurrentDisplay().setWindowSize(msg.width, msg.height);
    }

    RenderModule::RenderersCtx& RenderModule::getRenderers()
    {
        return m_renderers;
    }

}
