#pragma once

#include <aiko_types.h>
#include <bgfx/bgfx.h>

#include <types/textures.h>
#include "interfaces/itexture_impl.h"

namespace aiko::bgfx
{

    class BgfxTextureImpl : public interfaces::ITextureImpl
    {
    public:

        BgfxTextureImpl();
        virtual ~BgfxTextureImpl() override = default;

        virtual void use() override;
        virtual void unuse() override;
        virtual bool isValid() override;
        virtual uint id() override;

        // load
        virtual void create(int width, int height) override;
        virtual void load(const char*) override;
        virtual void unload() override;

        // Modify
        virtual void setPixels(std::vector<Color>& pixels) override;

    private:

        ::bgfx::TextureHandle m_textureHandle;
        texture::Texture m_texture;

    };
}
