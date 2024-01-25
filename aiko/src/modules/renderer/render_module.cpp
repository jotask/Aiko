#include "render_module.h"

#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>

#include "modules/renderer/render_component_2d.h"
#include "modules/renderer/render_component_3d.h"
#include "modules/renderer/render_component_texture.h"

RenderModule::RenderModule()
    : m_renderType(nullptr)
    , m_currentRenderType(RenderType::Texture)
{

}

void RenderModule::preInit()
{
    switch (m_currentRenderType)
    {
    case RenderModule::RenderType::TwoDimensions:
        m_renderType = std::make_unique<RenderComponent2D>();
        break;
    case RenderModule::RenderType::ThreeDimensions:
        m_renderType = std::make_unique<RenderComponent3D>();
        break;
    case RenderModule::RenderType::Texture:
        m_renderType = std::make_unique<RenderComponentTexture>();
        break;
    default:
        break;
    }
    m_renderType->preInit();
}

void RenderModule::preRender()
{
    ClearBackground(RAYWHITE);
}

void RenderModule::render()
{
    DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

    bool open = true;
    ImGui::ShowDemoWindow(&open);
    DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

}

void RenderModule::postRender()
{
}

void RenderModule::beginFrame()
{
    BeginDrawing();
    rlImGuiBegin();
}

void RenderModule::endFrame()
{
    DrawFPS(0, 0);
    rlImGuiEnd();
    EndDrawing();
}
