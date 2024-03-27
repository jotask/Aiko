#include "modules/platform_module.h"

#include <memory>

#include "modules/render_module.h"
#include "modules/module_connector.h"

#include "core/libs.h"
#include "models/time.h"

namespace aiko
{

    const char* PlatformModule::loadTextContent(char* filePath)
    {
        return nullptr;
    }

    void PlatformModule::preUpdate()
    {
        updateTime();
    }

    void PlatformModule::updateTime()
    {
        auto& time = Time::it();
        // time.deltaTime = ::GetFrameTime();
        // time.fps = ::GetFPS();
        time.frames++;
    }

}
