#include "render_module.h"

#include <string>

#include <core/libs.h>

#include "modules/renderer/render_component_2d.h"
#include "modules/renderer/render_component_3d.h"
#include "modules/renderer/render_component_texture.h"
#include "modules/renderer/render_component_pixel.h"
#include "modules/module_connector.h"
#include "modules/display_module.h"
#include "modules/camera_module.h"
#include "models/camera.h"
#include "core/textures.h"

#include "core/raylib_utils.h"

namespace aiko
{

    RenderModule::RenderModule()
        : m_renderType(nullptr)
        , m_currentRenderType(RenderType::ThreeDimensions)
        , m_isImguiDemoOpen(false)
        , m_displayModule(nullptr)
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
        m_displayModule = moduleConnector->find<DisplayModule>().get();
        m_cameraModule = moduleConnector->find<CameraModule>().get();
    }

    void RenderModule::preInit()
    {
        updateRenderType(m_currentRenderType, false);
        m_renderType->preInit();
    }
    
    void RenderModule::init()
    {

        auto size = getDisplaySize();
        auto data = LoadRenderTexture(size.x, size.y);
        m_renderTexture2D = raylib::utils::toRenderTexture2D(data);
        m_renderType->init();
    }
    
    void RenderModule::postInit()
    {
        m_renderType->postInit();
    }
    
    void RenderModule::preUpdate()
    {
        if (IsWindowResized() == true && IsWindowFullscreen() == false)
        {
            auto screenWidth = GetScreenWidth();
            auto screenHeight = GetScreenHeight();
            {
                auto rt = raylib::utils::toRaylibRenderTexture2D(m_renderTexture2D);
                UnloadRenderTexture(rt);
            }
            {
                auto texture = LoadRenderTexture(screenWidth, screenHeight);
                m_renderTexture2D = raylib::utils::toRenderTexture2D(texture);
            }
        }
        m_renderType->preUpdate();
    }
    
    void RenderModule::update()
    {
        m_renderType->update();
    }
    
    void RenderModule::postUpdate()
    {
        m_renderType->postUpdate();
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

        // We are done, render to the texture
        auto rt = raylib::utils::toRaylibRenderTexture2D(m_renderTexture2D);
        BeginTextureMode(rt);
        ClearBackground(::BLACK);
        DrawRectangle(0, 0, m_renderTexture2D.texture.width, m_renderTexture2D.texture.height, ::BLACK);
        EndTextureMode();

        m_renderType->preRender();
    }
    
    void RenderModule::render()
    {
    
        if (IsKeyPressed(KEY_F1))
        {
            m_isImguiDemoOpen = !m_isImguiDemoOpen;
            DrawText("Presse", 0, 20, 20, ::GREEN);
        }
    
        if (m_isImguiDemoOpen)
        {
            ImGui::ShowDemoWindow(&m_isImguiDemoOpen);
        }
    
        DrawText("Imgui Debug : " + m_isImguiDemoOpen ? "true" : "false", 0, 20, 20, ::GREEN);
    
        m_renderType->render();
    }
    
    void RenderModule::postRender()
    {
    
        m_renderType->postRender();
    }
    
    void RenderModule::beginFrame()
    {
        BeginDrawing();
        rlImGuiBegin();
        m_renderType->beginFrame();
    }
    
    void RenderModule::endFrame()
    {
        DrawFPS(0, 0);
        rlImGuiEnd();
        EndDrawing();
        m_renderType->endFrame();
    }

    ivec2 RenderModule::getDisplaySize()
    {
        return m_displayModule->getDisplaySize();
    }

    texture::RenderTexture2D* RenderModule::getRenderTexture()
    {
        return &m_renderTexture2D;
    }

    void RenderModule::updateRenderType(RenderModule::RenderType newRenderType, bool autoInit)
    {
        // TODO Do we want to do something with the previous if exists?
        m_currentRenderType = newRenderType;
        switch (m_currentRenderType)
        {
        case RenderModule::RenderType::TwoDimensions:
            m_renderType = std::make_unique<RenderComponent2D>(this);
            break;
        case RenderModule::RenderType::ThreeDimensions:
            m_renderType = std::make_unique<RenderComponent3D>(this);
            break;
        case RenderModule::RenderType::Texture:
            m_renderType = std::make_unique<RenderComponentTexture>(this);
            break;
        case RenderModule::RenderType::Pixel:
            m_renderType = std::make_unique<RenderComponentPixel>(this);
            break;
        default:
            // TODO assert
            break;
        }
        if (autoInit ==  true)
        {
            m_renderType->preInit();
            m_renderType->init();
            m_renderType->postInit();
        }
    }

    void RenderModule::clearBackground()
    {
        ClearBackground(::WHITE);
    }

    void RenderModule::beginMode2D()
    {
        // TODO We should pass the camera
        // auto* camera = m_cameraModule->GetMainCamera();
        // BeginMode2D( camera. );
    }

    void RenderModule::endMode2D()
    {
    }

    void RenderModule::beginMode3D()
    {
    }

    void RenderModule::endMode3D()
    {
    }

    void RenderModule::beginTextureMode(texture::RenderTexture2D* target)
    {
    }

    void RenderModule::endTextureMode(void)
    {
    }

    void RenderModule::beginShaderMode(Shader* shader)
    {
    }

    void RenderModule::endShaderMode(void)
    {
    }

    void RenderModule::beginBlendMode(int mode)
    {
    }

    void RenderModule::endBlendMode(void)
    {
    }

}
