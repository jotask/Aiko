#pragma once

#include <aiko_types.h>
#include <types/color.h>
#include <metadata/texture_meta.h>

#include "assets/types/texture_asset.h"
#include "types/render_types.h"

namespace aiko
{
    namespace interfaces
    {
        struct ITextureImpl
        {
            virtual ~ITextureImpl() = default;

            virtual RenderResourceId id() const = 0;
            virtual void use() = 0;
            virtual void unuse() = 0;
            virtual bool isValid() const = 0;
            virtual TextureInfo getInfo() = 0;

            // load
            virtual void create(const TextureDesc&) = 0;
            virtual void unload() = 0;

            // Modify
            virtual void update(const TextureAsset& asset) = 0;
            virtual void setPixels(const vector<Color>& pixels) = 0;

        };
    }
}
