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
        static constexpr const ImGuiWindowFlags flags = ImGuiWindowFlags_None;
        if (ImGui::Begin(name.c_str(), &is_open, flags))
        {

            NesEmulator* app = naiko->getApplication();

            ImGui::BeginChild("GameRender");

            ImVec2 textureSize = ImVec2( static_cast<float>(NES_WIDTH), static_cast<float>(NES_HEIGHT) );

            float aspectRatio = textureSize.x / textureSize.y;

            ImVec2 availableSpace = ImGui::GetContentRegionAvail();

            float maxWidth = availableSpace.x;
            float maxHeight = availableSpace.y;
            float imageWidth, imageHeight;

            if (maxWidth / maxHeight > aspectRatio)
            {
                imageHeight = maxHeight;
                imageWidth = maxHeight * aspectRatio;
            }
            else
            {
                imageWidth = maxWidth;
                imageHeight = maxWidth / aspectRatio;
            }

            imageWidth = std::min(imageWidth, maxWidth);
            imageHeight = std::min(imageHeight, maxHeight);

            ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x - imageWidth) * 0.5f, 0));
            ImGui::Image(static_cast<ImTextureID>( app->getImguiTextureId( app->getNesTextureId() ) ), ImVec2(imageWidth, imageHeight), ImVec2(0, 1), ImVec2(1, 0 ));
            ImGui::EndChild();

        }
        ImGui::End();
    }

    void GameWindow::onNesClock(const NesOnClockEvent& event)
    {
        AIKO_UNUSED(event);

        constexpr uint16_t ParticleCount = 1000;

        auto pixels = naiko->getPpu()->getPixels();

        auto randomPosition = []() -> aiko::vec2
        {
            return
            {
                static_cast<float>(aiko::utils::getRandomValue(0, NES_WIDTH - 1)),
                static_cast<float>(aiko::utils::getRandomValue(0, NES_HEIGHT - 1))
            };
        };

        auto randomVelocity = []() -> aiko::vec2
        {
            return
            {
                aiko::utils::getRandomValue(-1.0f, 1.0f),
                aiko::utils::getRandomValue(-1.0f, 1.0f)
            };
        };

        struct Particle
        {
            aiko::vec2 position;
            aiko::vec2 velocity;
        };

        static std::vector<Particle> particles(ParticleCount);
        static bool initialized = false;

        if (initialized == false)
        {
            initialized = true;

            for (Particle& particle : particles)
            {
                particle.position = randomPosition();
                particle.velocity = randomVelocity();
            }
        }

        for (Particle& particle : particles)
        {
            particle.position += particle.velocity;

            if (particle.position.x < 0.0f || particle.position.x >= static_cast<float>(NES_WIDTH))
            {
                particle.velocity.x = -particle.velocity.x;
                particle.position.x += particle.velocity.x;
            }

            if (particle.position.y < 0.0f || particle.position.y >= static_cast<float>(NES_HEIGHT))
            {
                particle.velocity.y = -particle.velocity.y;
                particle.position.y += particle.velocity.y;
            }

            const size_t x = static_cast<size_t>(particle.position.x);
            const size_t y = static_cast<size_t>(particle.position.y);

            const size_t index = y * static_cast<size_t>(NES_WIDTH) + x;

            if (index < pixels.size())
            {
                pixels[index] = aiko::WHITE;
            }
        }

        naiko->getApplication()->updateNesTexture(pixels);
    }

}
