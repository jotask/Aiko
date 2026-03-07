#pragma once

#include <aiko_types.h>

#include <models/texture.h>

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
            virtual void create(Texture color, Texture depth) = 0;
            virtual void unload() = 0;

        };
    }
}
