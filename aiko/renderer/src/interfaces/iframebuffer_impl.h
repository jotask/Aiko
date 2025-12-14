#pragma once

#include <vector>
#include <aiko_types.h>

#include "types/color.h"

namespace aiko
{
    namespace interfaces
    {
        struct IFrameBufferImpl
        {
            virtual ~IFrameBufferImpl() = default;

            virtual uint id() const = 0;
            virtual void use() = 0;
            virtual void unuse() = 0;
            virtual bool isValid() const = 0;

            // load
            virtual void create(int width, int height) = 0;
            virtual void unload() = 0;

            virtual ivec2 getSize() const = 0;

        };
    }
}
