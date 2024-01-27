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
    , m_isImguiDemoOpen(true)
{

}

void RenderModule::preInit()
{
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
    default:
        break;
    }
    m_renderType->preInit();
}

void RenderModule::init()
{
    m_renderType->init();
}

void RenderModule::postInit()
{
    m_renderType->postInit();
}

void RenderModule::preUpdate()
{
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
    m_renderType->preRender();
}

void RenderModule::render()
{

    if (IsKeyPressed(KEY_F1))
    {
        m_isImguiDemoOpen = !m_isImguiDemoOpen;
        DrawText("Presse", 0, 20, 20, GREEN);
    }

    if (m_isImguiDemoOpen)
    {
        ImGui::ShowDemoWindow(&m_isImguiDemoOpen);
    }

    DrawText("Imgui Debug : " + m_isImguiDemoOpen ? "true" : "false", 0, 20, 20, GREEN);

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
