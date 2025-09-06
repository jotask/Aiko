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

#include <bgfx/bgfx.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace aiko::bgfx
{

    void BgfxRenderModule::initScreenFbo()
    {
        AIKO_DEBUG_BREAK
    }

    texture::ScreenFbo BgfxRenderModule::getScreenFbo()
    {
        return m_screenFbo;
    }

    texture::Texture BgfxRenderModule::createTexture()
    {
        AIKO_DEBUG_BREAK
            return { 0 };
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
        AIKO_DEBUG_BREAK
            return { 0 };
    }

    void BgfxRenderModule::updatePboTexture(texture::PboTexture pbo, std::vector<Color>& pixels)
    {
        AIKO_DEBUG_BREAK
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