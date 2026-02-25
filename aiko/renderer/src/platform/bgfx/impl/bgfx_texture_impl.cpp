#include "bgfx_texture_impl.h"

#include <algorithm>
#include <random>

#include <logger/logger.h>
#include <core/file.h>

#include "constants.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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

    texture::Texture BgfxTextureImpl::getInfo()
    {
        return m_texture;
    }

    void BgfxTextureImpl::create(texture::Texture text)
    {

        AIKO_ASSERT(text.type != texture::TextureType::INVALID, "Invalid texture is not a valid texture type")

        uint64_t flags = 0;

        switch (text.type)
        {
        case texture::TextureType::Sampled:
            {
                flags = BGFX_TEXTURE_NONE;
            }
            break;
        case texture::TextureType::RenderTarget:
            {
                flags = BGFX_TEXTURE_RT;
            }
        case texture::TextureType::DepthStencil:
            {
                flags = BGFX_TEXTURE_RT;
            }
            break;
        default:
            AIKO_ASSERT(false, "Not supported texture type");
        }

        logger::Log::info("%d, %d", text.width, text.height);
        m_textureHandle = ::bgfx::createTexture2D(
            text.width,
            text.height,
            text.mipmaps,              // no mipmaps
            1,                         // layers
            toBGFXFormat(text.format),
            flags,          // flags, BGFX_TEXTURE_RT if you need a render target
            nullptr
        );
        AIKO_ASSERT(isValid(), "Invalid Texture");
        m_texture = text;
    }

    void BgfxTextureImpl::load(string file_path)
    {
        std::string base = global::GLOBAL_ASSET_PATH;
        base += std::string(file_path);

        AIKO_ASSERT(std::filesystem::exists(base), "Texture don't exist");

        int width, height, channels;
        // Load image data with stb_image
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(base.c_str(), &width, &height, &channels, 4); // force RGBA

        AIKO_ASSERT(data, "Texture Failed to load texture.")

        // Create bgfx memory from image data
        const ::bgfx::Memory* mem = ::bgfx::copy(data, width * height * 4);
        stbi_image_free(data);

        const auto textureFormat = texture::TextureFormat::BGRA8;

        // Create bgfx memory from image data
        m_textureHandle = ::bgfx::createTexture2D(
            width,
            height,
            false,            // no mipmaps for simplicity
            1,                // number of layers
            toBGFXFormat(textureFormat),
            0,                // flags
            mem
        );

        AIKO_ASSERT(::bgfx::isValid(m_textureHandle), "Invalid Texture");
        m_texture.width = width;
        m_texture.height = height;
        m_texture.mipmaps = false;
        m_texture.format = textureFormat;

    }

    void BgfxTextureImpl::unload()
    {
        ::bgfx::destroy(m_textureHandle);
        m_texture = {};
    }

    void BgfxTextureImpl::setPixels(std::vector<Color>& pixels)
    {

        const uint32_t pixelCount = m_texture.width * m_texture.height;

        // Allocate memory for RGBA8
        const auto channels = texture::getChannelCount(m_texture.format);
        const ::bgfx::Memory* newMem = ::bgfx::alloc(pixelCount * channels);

        if constexpr(false)
        {
            // Fill memory with initial data (optional)
            memset(newMem->data, 0x00, newMem->size);
        }
        else
        {
            uint8_t* dst = newMem->data;
            // Random number generator
            static std::mt19937 rng{ std::random_device{}() };
            static std::uniform_int_distribution<int> dist(0, 255);

            for (uint32_t i = 0; i < pixelCount; ++i)
            {
                Color c = pixels[i];
                dst[i * 4 + 0] = c.r * 255;
                dst[i * 4 + 1] = c.g * 255;
                dst[i * 4 + 2] = c.b * 255;
                dst[i * 4 + 3] = c.a * 255;
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

    ::bgfx::TextureFormat::Enum BgfxTextureImpl::toBGFXFormat(texture::TextureFormat format) const
    {
        switch (format)
        {
            case texture::TextureFormat::BGRA8: return ::bgfx::TextureFormat::BGRA8;
            case texture::TextureFormat::RGBA8: return ::bgfx::TextureFormat::RGBA8;
            case texture::TextureFormat::D24S8: return ::bgfx::TextureFormat::D24S8;
            default: AIKO_ASSERT(false, "Texture format not supported by backend")
        }
        return ::bgfx::TextureFormat::Enum::Unknown;
    }
}
