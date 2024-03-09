#ifdef AIKO_BUILD_RAYLIB
#include "modules/platform_module.h"

#include <memory>

#include "modules/render_module.h"
#include "modules/module_connector.h"

#include "platform/raylib/raylib_utils.h"
#include "core/libs.h"

namespace aiko
{

    const char* PlatformModule::loadTextContent(char* filePath)
    {
        const char* data = ::LoadFileText(filePath);
        return data;
    }

}
#endif
