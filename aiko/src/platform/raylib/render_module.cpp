#ifdef AIKO_BUILD_LIB
#include "modules/render_module.h"

#include <string>

#include <core/libs.h>

#include "modules/module_connector.h"
#include "modules/display_module.h"
#include "modules/camera_module.h"
#include "models/camera.h"
#include "types/textures.h"
#include "events/events.hpp"

#include "core/raylib_utils.h"

namespace aiko
{

    RenderModule::RenderModule()
        : m_displayModule(nullptr)
        , m_scale(false)
        , m_renderTexture2D()
    {
    
    }

    RenderModule::~RenderModule()
    {
        auto rt = raylib::utils::toRaylibRenderTexture2D(m_renderTexture2D);
        UnloadRenderTexture(rt);
    }
    
    void RenderModule::connect(ModuleConnector* moduleConnector)
    {
        BIND_MODULE_REQUIRED(DisplayModule, moduleConnector, m_displayModule)
        BIND_MODULE_REQUIRED(CameraModule, moduleConnector, m_cameraModule)
    }

    void RenderModule::preInit()
    {

    }
    
    void RenderModule::init()
    {
        auto size = getDisplaySize();
        auto data = LoadRenderTexture(size.x, size.y);
        m_renderTexture2D = raylib::utils::toRenderTexture2D(data);
        EventSystem::it().bind<WindowResizeEvent>(this, &RenderModule::onWindowResize);
    }
    
    void RenderModule::postInit()
    {

    }
    
    void RenderModule::preUpdate()
    {

    }
    
    void RenderModule::update()
    {

    }
    
    void RenderModule::postUpdate()
    {

    }
    
    void RenderModule::preRender()
    {
        /*
        if (::ImGui::Begin("RenderModule"))
        {

            auto stringToEnum = [](const char* str) -> RenderModule::RenderType
                {
                    if (strcmp(str, "TwoDimensions") == 0)      return RenderModule::RenderType::TwoDimensions;
                    if (strcmp(str, "ThreeDimensions") == 0)    return RenderModule::RenderType::ThreeDimensions;
                    if (strcmp(str, "Texture") == 0)            return RenderModule::RenderType::Texture;
                    if (strcmp(str, "Pixel") == 0)              return RenderModule::RenderType::Pixel;
                };

            auto enumToString = [](RenderModule::RenderType enm) -> const char*
                {
                    switch (enm)
                    {
                    case RenderModule::RenderType::TwoDimensions:   return "TwoDimensions";  
                    case RenderModule::RenderType::ThreeDimensions: return "ThreeDimensions";
                    case RenderModule::RenderType::Texture:         return "Texture";        
                    case RenderModule::RenderType::Pixel:           return "Pixel";          
                    default:
                        // TODO assert
                        break;
                    }
                };

            constexpr char* items[] = { "TwoDimensions" , "ThreeDimensions" , "Texture" , "Pixel" };
            static const char* current_item = enumToString(m_currentRenderType);

            if (ImGui::BeginCombo("##combo", current_item)) // The second parameter is the label previewed before opening the combo.
            {
                for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                {
                    bool is_selected = (current_item == items[n]); // You can store your selection however you want, outside or inside your objects

                    if (ImGui::Selectable(items[n], is_selected))
                    {
                        current_item = items[n];
                        RenderModule::RenderType enm = stringToEnum(current_item);
                        updateRenderType(enm);

                    }
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                    }
                }
                ImGui::EndCombo();
            }
            ::ImGui::End();
        }
        */
    }
    
    void RenderModule::render()
    {

    }
    
    void RenderModule::postRender()
    {

    }
    
    void RenderModule::beginFrame()
    {
        BeginDrawing();
        rlImGuiBegin();
        clearBackground(BLACK);
    }
    
    void RenderModule::endFrame()
    {

        ivec2 screenSize = getDisplaySize();
        vec2 targetSize = { static_cast<float>(m_renderTexture2D.texture.width), static_cast<float>(-m_renderTexture2D.texture.height) };
        vec2 targScreen = { static_cast<float>(screenSize.x), static_cast<float>(screenSize.y) };

        drawTexturePro(
            m_renderTexture2D.texture,
            Rectangle{ 0, 0, targetSize.x, targetSize.y },
            Rectangle{ 0, 0, targScreen.x, targScreen.y },
            { 0, 0 },
            0,
            WHITE
        );

        DrawFPS(0, 0);

        rlImGuiEnd();
        EndDrawing();
    }

    ivec2 RenderModule::getDisplaySize()
    {
        return m_displayModule->getCurrentDisplay().getDisplaySize();
    }

    texture::RenderTexture2D* RenderModule::getRenderTexture()
    {
        return &m_renderTexture2D;
    }

    void RenderModule::onWindowResize(Event& event)
    {
        const auto& msg = static_cast<const WindowResizeEvent&>(event);
        //resizeViewport(msg.width, msg.height);

        if (m_scale == true)
        {
            return;
        }

        auto screenWidth = msg.width;
        auto screenHeight = msg.height;
        {
            auto rt = raylib::utils::toRaylibRenderTexture2D(m_renderTexture2D);
            UnloadRenderTexture(rt);
        }
        {
            auto texture = LoadRenderTexture(screenWidth, screenHeight);
            m_renderTexture2D = raylib::utils::toRenderTexture2D(texture);
        }

    }

    void RenderModule::clearBackground(Color color)
    {
        auto c = raylib::utils::toRaylibColor(color);
        ClearBackground(c);
    }

    void RenderModule::beginMode2D(Camera* cam)
    {
        auto c = raylib::utils::toRaylibCamera3D(*cam);
        ::BeginMode3D(c);
    }

    void RenderModule::endMode2D()
    {
        ::EndMode2D();
    }

    void RenderModule::beginMode3D(Camera* cam)
    {
        auto c = raylib::utils::toRaylibCamera3D(*cam);
        ::BeginMode3D(c);
    }

    void RenderModule::endMode3D()
    {
        ::EndMode3D();
    }

    void RenderModule::beginTextureMode(texture::RenderTexture2D& target)
    {
        auto t = raylib::utils::toRaylibRenderTexture2D(target);
        ::BeginTextureMode(t);
    }

    void RenderModule::endTextureMode(void)
    {
        ::EndTextureMode();
    }

    void RenderModule::beginShaderMode(aiko::shader::Shader* shader)
    {
        auto s = raylib::utils::toRaylibShader( *shader );
        ::BeginShaderMode(s);
    }

    void RenderModule::endShaderMode(void)
    {
        ::EndShaderMode();
    }

    void RenderModule::beginBlendMode(BlendMode mode)
    {
        ::BeginBlendMode(mode);
    }

    void RenderModule::endBlendMode(void)
    {
        ::EndBlendMode();
    }

}
#endif
