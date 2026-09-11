#pragma once

#include <aiko_types.h>
#include <bgfx/bgfx.h>

#include <types/texture_types.h>
#include "interfaces/itexture_impl.h"

namespace aiko::renderer::bgfx
{

    class BgfxTextureImpl : public interfaces::ITextureImpl
    {
    public:

        BgfxTextureImpl();
        virtual ~BgfxTextureImpl() override = default;

        virtual RenderResourceId id() const override;
        virtual void use() override;
        virtual void unuse() override;
        virtual bool isValid() const override;
        virtual TextureInfo getInfo() override;

        // load
        virtual void create(const TextureDesc&) override;
        virtual void unload() override;

        // Modify
        virtual void update(const TextureAsset& asset) override;
        virtual void setPixels(const vector<Color>& pixels) override;

        ::bgfx::TextureHandle getTextureHandler() const { return m_textureHandle; }
        // uint64_t getSamplerFlags() const;

    private:

        ::bgfx::TextureHandle m_textureHandle;
        TextureInfo m_texture;

        ::bgfx::TextureFormat::Enum toBGFXFormat(TextureFormat) const;


    };
}
