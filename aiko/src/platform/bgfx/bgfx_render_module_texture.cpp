#ifdef AIKO_BGFX

#include "bgfx_render_module.h"

#include <fstream>
#include <filesystem>

#include "core/libs.h"

#include "modules/module_connector.h"
#include "modules/display_module.h"
#include "models/camera.h"
#include "types/textures.h"
#include "events/events.hpp"
#include "types/render_types.h"
#include "models/mesh.h"
#include "models/shader.h"
#include "components/transform_component.h"
#include "constants.h"
#include "platform/bgfx/bgfx_platform_helper.h"

#include <bgfx/bgfx.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace aiko::bgfx
{

    void BgfxRenderModule::initScreenFbo()
    {

        // Postprocessing shader
        m_passthrought.load("posprocessing");
        if (m_passthrought.isvalid() == false)
        {
            int a = 9;
        }
        m_passthrought.preLoadUniforms({ {"u_texture", ShaderUniformDataType::SHADER_UNIFORM_SAMPLER2D} });

        // VAO// IBH -> Not Needed we are rendering to full 

        // FBO

        auto size = m_displayModule->getCurrentDisplay().getDisplaySize();

        // Example: create a 2D color texture
        ::bgfx::TextureHandle colorTex = ::bgfx::createTexture2D(
            size.x, size.y,             // width, height
            false,                 // hasMips
            1,                     // numLayers
            ::bgfx::TextureFormat::RGBA8,
            BGFX_TEXTURE_RT        // mark as render target
        );

        // Example: create a depth buffer texture
        ::bgfx::TextureHandle depthTex = ::bgfx::createTexture2D(
            size.x, size.y,
            false,
            1,
            ::bgfx::TextureFormat::D24S8,
            BGFX_TEXTURE_RT
        );

        std::vector<::bgfx::TextureHandle> fbTextures = { colorTex, depthTex };
        ::bgfx::FrameBufferHandle myFbo = ::bgfx::createFrameBuffer(fbTextures.size(), fbTextures.data(), true);

        texture::RenderTexture2D& target = m_screenFbo.renderTexture;
        target.framebuffer = myFbo.idx;

        target.texture.id = colorTex.idx;
        target.texture.width = size.x;
        target.texture.height = size.y;
        target.texture.format = ::bgfx::TextureFormat::RGBA8;

        target.depth.id = depthTex.idx;
        target.depth.width = size.x;
        target.depth.height = size.y;
        target.depth.format = ::bgfx::TextureFormat::D24S8;

    }

    texture::ScreenFbo BgfxRenderModule::getScreenFbo()
    {
        return m_screenFbo;
    }

    texture::Texture BgfxRenderModule::createTexture()
    {
        return createTexture(1, 1);
    }

    texture::Texture BgfxRenderModule::createTexture(int width, int height)
    {

        const uint32_t pixelCount = width * height;

        
        // Create texture using memory
        ::bgfx::TextureHandle tex = ::bgfx::createTexture2D(
            width,
            height,
            false,                     // no mipmaps
            1,                         // layers
            ::bgfx::TextureFormat::RGBA8,
            BGFX_TEXTURE_NONE,          // flags, BGFX_TEXTURE_RT if you need a render target
            nullptr
        );

        if (::bgfx::isValid(tex) == false)
        {
            std::runtime_error("Invalid texture");
        }

        texture::Texture texture = {0};

        texture.id = tex.idx;
        texture.width = width;
        texture.height = height;
        texture.channels = 4;
        texture.mipmaps = 1;
        texture.format = ::bgfx::TextureFormat::RGBA8;

        return texture;

    }

    texture::Texture BgfxRenderModule::loadTexture(const char* file)
    {

        std::string base = ::aiko::global::GLOBAL_ASSET_PATH;
        base += std::string(file);

        if (std::filesystem::exists(base) == false)
        {
            return {0};
        }

        int width, height, channels;
        // Load image data with stb_image
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(base.c_str(), &width, &height, &channels, 4); // force RGBA
        if (!data)
        {
            printf("Failed to load texture: %s\n", file);
            std::runtime_error("Error!");
            return { 0 };
        }
        
        // Create bgfx memory from image data
        const ::bgfx::Memory* mem = ::bgfx::copy(data, width * height * 4);
        
        // Create the texture
        ::bgfx::TextureHandle handle = ::bgfx::createTexture2D(
            uint16_t(width),
            uint16_t(height),
            false,            // no mipmaps for simplicity
            1,                // number of layers
            ::bgfx::TextureFormat::BGRA8,
            0,                // flags
            mem
        );
        
        stbi_image_free(data);
        
        texture::Texture texture;
        
        texture.id = handle.idx;
        texture.width = width;
        texture.height = height;
        texture.channels = channels;
        
        texture.mipmaps = 1;
        texture.format = ::bgfx::TextureFormat::BGRA8;
        
        return texture;

    }

    texture::PboTexture BgfxRenderModule::createPboTexture(uint16_t width, uint16_t height)
    {
        texture::PboTexture pbo = { 0 };
        pbo.texture = createTexture(width, height);
        return pbo;
    }

    void BgfxRenderModule::updatePboTexture(texture::PboTexture pbo, std::vector<Color>& pixels)
    {

        const uint32_t pixelCount = pbo.texture.width * pbo.texture.height;

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
            AIKO_TO_TH(pbo.texture.id),
            0, 0, 0, 0,
            pbo.texture.width,
            pbo.texture.height,
            newMem
        );
    }

    void BgfxRenderModule::drawTextureEx(texture::Texture texture, vec2 position, float rotation, float scale, Color tint)
    {
        AIKO_DEBUG_BREAK
    }

    void BgfxRenderModule::drawRenderTextureEx(texture::RenderTexture2D texture, vec2 position, float rotation, float scale, Color tint)
    {
        AIKO_DEBUG_BREAK
    }

}
#endif