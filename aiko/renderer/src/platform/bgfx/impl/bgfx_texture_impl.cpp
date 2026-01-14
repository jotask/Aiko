#include "bgfx_texture_impl.h"

#include <algorithm>
#include <random>

#include <logger/logger.h>
#include <core/file.h>

#include "constants.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace aiko::bgfx
{

    BgfxTextureImpl::BgfxTextureImpl()
        : m_textureHandle({::bgfx::kInvalidHandle})
        , m_texture ( {0})
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

    void BgfxTextureImpl::create(int width, int height)
    {
        m_textureHandle = ::bgfx::createTexture2D(
            width,
            height,
            false,                     // no mipmaps
            1,                         // layers
            ::bgfx::TextureFormat::RGBA8,
            BGFX_TEXTURE_NONE,          // flags, BGFX_TEXTURE_RT if you need a render target
            nullptr
        );
        AIKO_ASSERT(isValid(), "Invalid Texture");
        m_texture.id = m_textureHandle.idx;
        m_texture.width = width;
        m_texture.height = height;
        m_texture.mipmaps = false;
        m_texture.channels = 4;
        m_texture.format = ::bgfx::TextureFormat::RGBA8;
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

        // Create bgfx memory from image data
        m_textureHandle = ::bgfx::createTexture2D(
            width,
            height,
            false,            // no mipmaps for simplicity
            1,                // number of layers
            ::bgfx::TextureFormat::BGRA8,
            0,                // flags
            mem
        );

        AIKO_ASSERT(::bgfx::isValid(m_textureHandle), "Invalid Texture");

        m_texture.id = m_textureHandle.idx;
        m_texture.width = width;
        m_texture.height = height;
        m_texture.mipmaps = false;
        m_texture.channels = 4;
        m_texture.format = ::bgfx::TextureFormat::BGRA8;

    }

    void BgfxTextureImpl::unload()
    {
        ::bgfx::destroy(m_textureHandle);
        m_texture = {0};
    }

    void BgfxTextureImpl::setPixels(std::vector<Color> &pixels)
    {

        const uint32_t pixelCount = m_texture.width * m_texture.height;

        // Allocate memory for RGBA8
        const ::bgfx::Memory* newMem = ::bgfx::alloc(pixelCount * 4);

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
}
