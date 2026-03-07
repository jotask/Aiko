#pragma once

#include <vector>
#include <aiko_types.h>
#include <types/color.h>
#include <metadata/texture_meta.h>

namespace aiko
{
    namespace interfaces
    {
        struct ITextureImpl
        {
            virtual ~ITextureImpl() = default;

            virtual uint id() const = 0;
            virtual void use() = 0;
            virtual void unuse() = 0;
            virtual bool isValid() const = 0;
            virtual TextureInfo getInfo() = 0;

            // load
            virtual void create(const TextureDesc&) = 0;
            virtual void unload() = 0;

            // Modify
            virtual void setPixels(const std::vector<Color>& pixels) = 0;

        };
    }
}
