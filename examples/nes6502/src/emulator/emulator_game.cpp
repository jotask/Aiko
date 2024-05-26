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

        constexpr const uint16_t PARTICLES_AMOUNT = 1000;

        aiko::PboTextureComponent* pbo = naiko->getApplication()->getNesGo();

        auto randomPosition = [&]() -> aiko::vec2
        {
            return aiko::vec2
            (
                aiko::utils::getRandomValue(0, NES_WIDTH - 1),
                aiko::utils::getRandomValue(0, NES_HEIGHT - 1)
            );
        };

        auto randomVelocity = [&]() -> aiko::vec2
        {
            return aiko::vec2
            (
                aiko::utils::getRandomValue(-1.0f, 1.0f),
                aiko::utils::getRandomValue(-1.0f, 1.0f)
            );
        };

        struct Particle { aiko::vec2 p; aiko::vec2 v; };

        static std::vector<Particle> particles(PARTICLES_AMOUNT);
        bool static first = true;
        if (first)
        {
            first = false;
            for (auto& p : particles)
            {
                p.p = randomPosition();
                p.v = randomVelocity();
            }
        }

        for (auto& p : particles)
        {

            // Clear the previous pixel
            pbo->updatePixel(p.p.x, p.p.y, aiko::BLACK);

            // Update the position
            p.p.x += p.v.x;
            p.p.y += p.v.y;

            // Check for boundary collision and reverse direction if necessary
            if (p.p.x < 0 || p.p.x >= pbo->getPboTexture().texture.width)
            {
                p.v.x = -p.v.x;
                p.p.x += p.v.x; // Correct the position after reversing direction
            }
            if (p.p.y < 0 || p.p.y >= pbo->getPboTexture().texture.height)
            {
                p.v.y = -p.v.y;
                p.p.y += p.v.y; // Correct the position after reversing direction
            }

            // Draw the new pixel
            pbo->updatePixel(p.p.x, p.p.y, aiko::WHITE);

        }

        pbo->refreshPixels();
    }

}
