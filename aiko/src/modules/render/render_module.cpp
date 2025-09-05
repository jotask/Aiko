#include "render_module.h"

#include "modules/module_connector.h"
#include "modules/display_module.h"
#include "events/events.hpp"

#include "models/shader.h"
#include "models/mesh.h"

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

        // This just allow us to create instances, without coupling to all system/modules fnt calls
        Mesh::s_renderModule = this;
        Shader::s_renderModule = this;

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
