#include "display_module.h"

#include "aiko.h"

#include <display/display_manager.h>

namespace aiko
{
    DisplayModule::DisplayModule(Aiko* aiko)
        : BaseModule(aiko)
        // , m_manager(nullptr)
    {
    }

    const ivec2 DisplayModule::getDisplaySize() const
    {
        return DisplayManager::it().getDisplay()->getDisplaySize();
    }

    void DisplayModule::preInit()
    {
        const AikoConfig cfg = getAiko()->getConfig();
        DisplayManager::it().init(cfg.window_tittle, cfg.width, cfg.height);
    }

    void DisplayModule::preUpdate()
    {
        DisplayManager::it().update();
    }

    void DisplayModule::endFrame()
    {
        DisplayManager::it().swap();
    }

}
