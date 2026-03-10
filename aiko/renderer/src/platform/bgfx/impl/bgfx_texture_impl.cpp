#include "bgfx_texture_impl.h"

#include <algorithm>
#include <random>

#include <logger/logger.h>
#include <core/file.h>

#include "constants.h"

namespace aiko::renderer::bgfx
{

    BgfxTextureImpl::BgfxTextureImpl()
        : m_textureHandle({::bgfx::kInvalidHandle})
        , m_texture ( {})
    {
    }

    uint BgfxTextureImpl::id() const
    {
        return m_textureHandle.idx;
    }

    void BgfxTextureImpl::use()
    {

    }

    void BgfxTextureImpl::unuse()
    {

    }

    bool BgfxTextureImpl::isValid() const
    {
        return ::bgfx::isValid(m_textureHandle);
    }

    TextureInfo BgfxTextureImpl::getInfo()
    {
        return m_texture;
    }

    void BgfxTextureImpl::create(const TextureDesc& desc)
    {

        unload();

        const bool hasMips = desc.mipmaps > 1;

        uint64_t flags = BGFX_TEXTURE_NONE;

        switch (desc.type)
        {
        case TextureType::Sampled:
            {
                flags = BGFX_TEXTURE_NONE;
            }
            break;
        case TextureType::RenderTarget:
            {
                flags = BGFX_TEXTURE_RT;
            }
            break;
        case TextureType::DepthStencil:
            {
                flags = BGFX_TEXTURE_RT;
            }
            break;
        default:
            AIKO_ASSERT(false, "Not supported texture type");
        }

        if (desc.type == TextureType::DepthStencil)
        {
            AIKO_ASSERT(
                desc.format == TextureFormat::D24S8 ||
                desc.format == TextureFormat::D32F,
                "Depth texture must use depth format"
            );
        }

        if (desc.computeWrite == true)
        {
            flags |= BGFX_TEXTURE_COMPUTE_WRITE;
        }

        m_textureHandle = ::bgfx::createTexture2D(
            desc.width,
            desc.height,
            hasMips,
            1,
            toBGFXFormat(desc.format),
            flags,
            nullptr
        );
        AIKO_ASSERT(isValid(), "Invalid Texture");
        m_texture =
        {
            .type = desc.type,
            .format = desc.format,
            .width = desc.width,
            .height = desc.height,
            .mipmaps = desc.mipmaps,
            .computeWrite = desc.computeWrite,
            .valid = isValid()
        };
    }

    void BgfxTextureImpl::unload()
    {
        if (isValid() == true)
        {
            ::bgfx::destroy(m_textureHandle);
            m_textureHandle = { :: bgfx::kInvalidHandle };
        }
        AIKO_ASSERT(isValid() == false, "Valid texture after unload?")
        m_texture = {};
    }

    void BgfxTextureImpl::update(const TextureAsset& asset)
    {
        setPixels(asset.pixels);
    }

    void BgfxTextureImpl::setPixels(const std::vector<Color>& pixels)
    {

        AIKO_ASSERT(isValid(), "Invalid texture");
        AIKO_ASSERT(m_texture.width > 0 && m_texture.height > 0 , "Invalid texture size");

        const uint32_t pixelCount = m_texture.width * m_texture.height;

        AIKO_ASSERT(pixels.size() == pixelCount, "Mismatch pixels count and pixel size");

        // Allocate memory for RGBA8
        const auto channels = getChannelCount(m_texture.format);
        AIKO_ASSERT(channels == 4, "Not supported, we only support 4 channesl for now");

        const ::bgfx::Memory* newMem = ::bgfx::alloc(pixelCount * channels);

        if constexpr(false)
        {
            // Fill memory with initial data (optional)
            memset(newMem->data, 0x00, newMem->size);
        }
        else
        {
            uint8_t* dst = newMem->data;

            for (uint32_t i = 0; i < pixelCount; ++i)
            {
                Color c = pixels[i];
                dst[i * 4 + 0] = static_cast<uint8_t>(std::clamp(c.r, 0.0f, 1.0f) * 255.0f);
                dst[i * 4 + 1] = static_cast<uint8_t>(std::clamp(c.g, 0.0f, 1.0f) * 255.0f);
                dst[i * 4 + 2] = static_cast<uint8_t>(std::clamp(c.b, 0.0f, 1.0f) * 255.0f);
                dst[i * 4 + 3] = static_cast<uint8_t>(std::clamp(c.a, 0.0f, 1.0f) * 255.0f);
            }
        }

        ::bgfx::updateTexture2D(
            m_textureHandle,
            0, 0, 0, 0,
            m_texture.width,
            m_texture.height,
            newMem
        );
    }

    /*
    uint64_t BgfxTextureImpl::getSamplerFlags() const
    {

        uint64_t flags = 0;

        if (m_texture.minFilter == TextureFilter::Nearest)
        {
            flags |= BGFX_SAMPLER_MIN_POINT;
        }

        if (m_texture.magFilter == TextureFilter::Nearest)
        {
            flags |= BGFX_SAMPLER_MAG_POINT;
        }

        if (m_texture.mipFilter == TextureMipFilter::Nearest)
        {
            flags |= BGFX_SAMPLER_MIP_POINT;
        }

        auto wrapToFlags = [](TextureWrapMode w, bool isU) -> uint64_t
        {
            switch (w)
            {
            case TextureWrapMode::Repeat: return 0;
            case TextureWrapMode::Clamp:  return isU ? BGFX_SAMPLER_U_CLAMP : BGFX_SAMPLER_V_CLAMP;
            case TextureWrapMode::Mirror: return isU ? BGFX_SAMPLER_U_MIRROR: BGFX_SAMPLER_V_MIRROR;
            }
            return 0;
        };

        flags |= wrapToFlags(m_texture.wrapU, true);
        flags |= wrapToFlags(m_texture.wrapV, false);

        if (m_texture.anisotropy > 1 && m_texture.minFilter == TextureFilter::Linear)
        {
            flags |= BGFX_SAMPLER_MIN_ANISOTROPIC;
        }

        return flags;
    }
    */

    ::bgfx::TextureFormat::Enum BgfxTextureImpl::toBGFXFormat(TextureFormat format) const
    {
        switch (format)
        {
            case TextureFormat::BGRA8: return ::bgfx::TextureFormat::BGRA8;
            case TextureFormat::RGBA8: return ::bgfx::TextureFormat::RGBA8;
            case TextureFormat::D24S8: return ::bgfx::TextureFormat::D24S8;
            default: AIKO_ASSERT(false, "Texture format not supported by backend")
        }
        return ::bgfx::TextureFormat::Enum::Unknown;
    }
}
