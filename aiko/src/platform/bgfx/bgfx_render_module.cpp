#ifdef AIKO_BGFX

#include "bgfx_render_module.h"

#include <fstream>

#include <iostream>
#include <set>
#include <vector>

#include <algorithm>

#include "aiko.h"
#include "constants.h"
#include "modules/module_connector.h"
#include "modules/display_module.h"
#include "types/textures.h"
#include "events/events.hpp"
#include "types/render_types.h"
#include "models/mesh.h"
#include "models/shader.h"
#include "components/transform_component.h"
#include "modules/render/render_primitives.h"
#include "core/libs.h"
#include "core/log.h"
#include "constants.h"

#include "platform/opengl/opengl_2d_renderer.h"
#include "platform/opengl/opengl_3d_renderer.h"

#include "modules/render/render_primitives.h"

namespace aiko::bgfx
{

    BgfxRenderModule::BgfxRenderModule(Aiko* aiko)
        : RenderModule(aiko)
    {
    
    }

    BgfxRenderModule::~BgfxRenderModule()
    {
        
    }


    void BgfxRenderModule::preInit()
    {
        AIKO_NOT_IMPLEMENTED
    }

    void BgfxRenderModule::init()
    {
        AIKO_NOT_IMPLEMENTED
    }

    void BgfxRenderModule::beginFrame()
    {
        AIKO_NOT_IMPLEMENTED
    }

    void BgfxRenderModule::endFrame()
    {
        AIKO_NOT_IMPLEMENTED
    }

    void BgfxRenderModule::drawText(string texto, float x, float y , float scale, Color color)
    {

    }

    void BgfxRenderModule::dispose()
    {
        AIKO_NOT_IMPLEMENTED
    }

    texture::RenderTexture2D* BgfxRenderModule::getRenderTexture()
    {
        return &m_screenFbo.renderTexture;
    }

    void BgfxRenderModule::onWindowResize(Event& event)
    {

        RenderModule::onWindowResize(event);

        const auto& msg = static_cast<const WindowResizeEvent&>(event);

        if (m_scale == true)
        {
            return;
        }

        const auto screenWidth = msg.width;
        const auto screenHeight = msg.height;

        AIKO_NOT_IMPLEMENTED

    }

    void BgfxRenderModule::clearBackground(Color color)
    {
        AIKO_NOT_IMPLEMENTED
    }

    void BgfxRenderModule::beginMode2D()
    {
        AIKO_NOT_IMPLEMENTED
    }

    void BgfxRenderModule::endMode2D()
    {

        AIKO_NOT_IMPLEMENTED
    }

    void BgfxRenderModule::beginMode3D()
    {
        AIKO_NOT_IMPLEMENTED
    }

    void BgfxRenderModule::endMode3D()
    {

    }

    void BgfxRenderModule::beginTextureMode()
    {

        AIKO_NOT_IMPLEMENTED
    }

    void BgfxRenderModule::beginTextureMode(texture::RenderTexture2D& target)
    {
        AIKO_NOT_IMPLEMENTED
    }

    void BgfxRenderModule::endTextureMode(void)
    {
        AIKO_NOT_IMPLEMENTED
    }

    void BgfxRenderModule::beginShaderMode(aiko::Shader* shader)
    {
        AIKO_NOT_IMPLEMENTED
    }

    void BgfxRenderModule::endShaderMode(void)
    {
        AIKO_NOT_IMPLEMENTED
    }

    void BgfxRenderModule::beginBlendMode(BlendMode mode)
    {

    }

    void BgfxRenderModule::endBlendMode(void)
    {

    }

}
#endif