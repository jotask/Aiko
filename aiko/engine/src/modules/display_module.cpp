#include "display_module.h"

#include "aiko.h"

#include <display/display_manager.h>

namespace aiko
{
    void DisplayModule::preInit()
    {
        const AikoConfig cfg = getAiko()->getConfig();
        DisplayManager::it().init(cfg.window_tittle, cfg.width, cfg.height);
    }

}
