#include "display_module.h"

#include "events/events.hpp"

namespace aiko
{
    void DisplayModule::preInit()
    {
        EventSystem::it().bind<WindowResizeEvent>(this, &DisplayModule::onWindowResize);

        const AikoConfig cfg = getAiko()->getConfig();
        m_curent.setWindowSize(cfg.width, cfg.height);
        m_curent.setWindowTitle(cfg.window_tittle.c_str());
    }

    void* DisplayModule::getNativeDisplay()
    {
        return m_curent.getNative();
    }

    void DisplayModule::onWindowResize(Event& event)
    {
        const auto& msg = static_cast<const WindowResizeEvent&>(event);
        m_curent.setWindowSize( msg.width, msg.height );
    }

}
