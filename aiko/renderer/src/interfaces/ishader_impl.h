#pragma once

#include <aiko_types.h>

#include "types/render_types.h"

namespace aiko
{
    namespace interfaces
    {
        struct IShaderImpl
        {
            virtual ~IShaderImpl() = default;

            virtual RenderResourceId id() const = 0;
            virtual bool isValid() const = 0;

            // load
            virtual void load(const char*, const char*) = 0;
            virtual void unload() = 0;

        };
    }
}
