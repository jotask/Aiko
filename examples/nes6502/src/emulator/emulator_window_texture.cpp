#include "emulator_window_texture.h"

#include "emulator/emulator.h"
#include "nes_emulator.h"

#include <aiko_includes.h>
#include <imgui.h>

namespace nes
{
    GameWindow::GameWindow(Naiko* n)
        : EmulatorWindow(n, "Game")
    {

    }

    void GameWindow::update()
    {

    }

    void GameWindow::render()
    {
        ImGui::SetNextWindowSize(ImVec2(256, 240));
        if (ImGui::Begin(name.c_str(), &is_open))
        {
            aiko::texture::RenderTexture2D* texture = naiko->getApplication()->getTargetTexture();
            // Using a Child allow to fill all the space of the window.
            // It also allows customization
            ImGui::BeginChild("GameRender");
            // Get the size of the child (i.e. the whole draw size of the windows).
            ImVec2 wsize = ImGui::GetWindowSize();
            // Because I use the texture from OpenGL, I need to invert the V from the UV.
            ImGui::Image((ImTextureID)texture->texture, wsize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::EndChild();
        }
        ImGui::End();
    }

}
