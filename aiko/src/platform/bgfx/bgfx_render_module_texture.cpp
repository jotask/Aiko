#ifdef AIKO_BGFX

#include "bgfx_render_module.h"

#include <fstream>

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