#include "render_module.h"

#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>

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
