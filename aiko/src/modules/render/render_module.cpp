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

    }

    void RenderModule::beginFrame()
    {

    }

    void RenderModule::endFrame()
    {

    }

    void RenderModule::dispose()
    {

    }

    ivec2 RenderModule::getDisplaySize()
    {
        return m_displayModule->getCurrentDisplay().getDisplaySize();
    }

    void RenderModule::onWindowResize(Event& event)
    {
        const auto& msg = static_cast<const WindowResizeEvent&>(event);
        m_displayModule->getCurrentDisplay().setWindowSize(msg.width, msg.height);
    }

}
