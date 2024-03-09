#pragma once

#include <map>

#include "aiko_types.h"
#include "modules/base_module.h"
#include "types/asset_type.h"
#include "models/shader.h"

namespace aiko
{
    class PlatformModule : public BaseModule
    {
    public:

        PlatformModule() = default;
        virtual ~PlatformModule() = default;

        const char* loadTextContent(char* filePath);

    };

}
