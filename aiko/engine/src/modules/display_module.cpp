#include "display_module.h"

#include <events/events.hpp>
#include <display/display_events.hpp>
#include <display/display_manager.h>

#include "aiko.h"
#include "aiko_config.h"

namespace aiko
{
    void DisplayModule::preInit()
    {
        EventSystem::it().bind<WindowResizeEvent>(this, &DisplayModule::onWindowResize);
        const AikoConfig cfg = getAiko()->getConfig();
        DisplayManager::it().init(cfg.window_tittle, cfg.width, cfg.height);
    }

    void DisplayModule::onWindowResize(Event& event)
    {
        const auto& msg = static_cast<const WindowResizeEvent&>(event);
        m_curent.setWindowSize( msg.width, msg.height );
    }

}
