#include "display_module.h"

#include "aiko.h"

#include <display/display_manager.h>

namespace aiko
{
    DisplayModule::DisplayModule(Aiko* aiko)
        : BaseModule(aiko)
    {
    }

    ivec2 DisplayModule::getDisplaySize() const
    {
        return m_manager.getDisplay()->getDisplaySize();
    }

    void* DisplayModule::getNativeWindow() const
    {
        return m_manager.getNativeWindow();
    }

    void DisplayModule::preInit()
    {
        const AikoConfig& cfg = getAiko()->getConfig();
        m_manager.init(cfg.windowTitle, cfg.width, cfg.height);
    }

    void DisplayModule::preUpdate()
    {
        m_manager.update();
    }

    void DisplayModule::endFrame()
    {
        m_manager.swap();
    }

}
