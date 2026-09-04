#pragma once

#include <aiko_types.h>

#include "interfaces/itexture_impl.h"

namespace aiko
{
    namespace interfaces
    {
        struct IFrameBufferImpl
        {
            virtual ~IFrameBufferImpl() = default;

            virtual RenderResourceId id() const = 0;
            virtual bool isValid() const = 0;

            // load
            virtual void create(ITextureImpl& color, ITextureImpl& depth) = 0;
            virtual void unload() = 0;

        };
    }
}
