#include "emulator_game.h"

#include "emulator/emulator.h"
#include "nes_emulator.h"
#include "aiko_extensions/nes_emulator_component.h"

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
        if (ImGui::Begin(name.c_str(), &is_open))
        {
            auto component = naiko->getApplication()->getNesGo();
            aiko::texture::Texture texture = component->getTexture();
            // Using a Child allow to fill all the space of the window.
            // It also allows customization
            ImGui::BeginChild("GameRender");
            // Get the size of the child (i.e. the whole draw size of the windows).

                        // Get the dimensions of the texture
            ImVec2 textureSize = ImVec2(texture.width, texture.height);

            // Calculate aspect ratio of the image
            float aspectRatio = textureSize.x / textureSize.y;

            // Get available space in the window
            ImVec2 availableSpace = ImGui::GetContentRegionAvail();

            // Calculate the maximum size of the image while maintaining the aspect ratio
            float maxWidth = availableSpace.x;
            float maxHeight = availableSpace.y;
            float imageWidth, imageHeight;

            if (maxWidth / maxHeight > aspectRatio)
            {
                // Height is the limiting factor
                imageHeight = maxHeight;
                imageWidth = maxHeight * aspectRatio;
            }
            else
            {
                // Width is the limiting factor
                imageWidth = maxWidth;
                imageHeight = maxWidth / aspectRatio;
            }

            // Ensure that the image does not exceed the available space
            imageWidth = std::min(imageWidth, maxWidth);
            imageHeight = std::min(imageHeight, maxHeight);

            ImGui::Image((ImTextureID)texture.id, ImVec2(imageWidth, imageHeight), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::EndChild();

        }
        ImGui::End();
    }

}
