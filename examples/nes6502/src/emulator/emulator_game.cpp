#include "emulator_game.h"

#include "emulator/emulator.h"
#include "nes_emulator.h"

#include "aiko_extensions/nes_events.h"

#include <aiko_includes.h>
#include <imgui.h>

namespace nes
{
    GameWindow::GameWindow(Naiko* n)
        : EmulatorWindow(n, "Game")
    {
        aiko::EventSystem::it().bind<NesOnClockEvent>(this, &GameWindow::onNesClock);
    }

    void GameWindow::update()
    {

    }

    void GameWindow::render()
    {
        if (ImGui::Begin(name.c_str(), &is_open))
        {

            aiko::PboTextureComponent* pbo = naiko->getApplication()->getNesGo();
            aiko::texture::Texture texture = pbo->getPboTexture().texture;
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

            ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x - imageWidth) * 0.5f, 0));
            ImGui::Image((ImTextureID)texture.id, ImVec2(imageWidth, imageHeight), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::EndChild();

        }
        ImGui::End();
    }

    void GameWindow::onNesClock(aiko::Event& event)
    {
        aiko::PboTextureComponent* pbo = naiko->getApplication()->getNesGo();
        static int x = 0;
        static int y = pbo->getPboTexture().texture.height / 2;
        pbo->updatePixel(x, y, aiko::BLACK);
        x++;
        x %= pbo->getPboTexture().texture.width;
        pbo->updatePixel(x, y, aiko::WHITE);
        pbo->refreshPixels();
    }

}
