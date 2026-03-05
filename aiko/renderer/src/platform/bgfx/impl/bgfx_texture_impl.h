#pragma once

#include <aiko_types.h>
#include <bgfx/bgfx.h>

#include <types/textures.h>
#include "interfaces/itexture_impl.h"

namespace aiko::renderer::bgfx
{

    class BgfxTextureImpl : public interfaces::ITextureImpl
    {
    public:

        BgfxTextureImpl();
        virtual ~BgfxTextureImpl() override = default;

        virtual uint id() const override;
        virtual void use() override;
        virtual void unuse() override;
        virtual bool isValid() const override;
        virtual texture::Texture& getInfo() override;

        // load
        virtual void create(texture::Texture) override;
        virtual void load(string) override;
        virtual void unload() override;

        // Modify
        virtual void setPixels(std::vector<Color>& pixels) override;

        ::bgfx::TextureHandle getTextureHandler() const { return m_textureHandle; }
        uint64_t getSamplerFlags() const;

    private:

        ::bgfx::TextureHandle m_textureHandle;
        texture::Texture m_texture;

        ::bgfx::TextureFormat::Enum toBGFXFormat(texture::TextureFormat) const;


    };
}
