#pragma once

#include <vector>
#include <aiko_types.h>

#include "types/color.h"

namespace aiko
{
    namespace interfaces
    {
        struct AIKO_API ITextureImpl
        {
            virtual ~ITextureImpl() = default;

            virtual uint id() const = 0;
            virtual void use() = 0;
            virtual void unuse() = 0;
            virtual bool isValid() const = 0;
            virtual texture::Texture getInfo() = 0;

            // load
            virtual void create(int width, int height) = 0;
            virtual void load(string) = 0;
            virtual void unload() = 0;

            // Modify
            virtual void setPixels(std::vector<Color>& pixels) = 0;

        };
    }
}
