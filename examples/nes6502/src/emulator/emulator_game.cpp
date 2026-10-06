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

            auto pbo = naiko->getApplication()->getNesGo();

            ImGui::BeginChild("GameRender");

            ImVec2 textureSize = ImVec2( static_cast<float>(pbo->getWidth()), static_cast<float>(pbo->getHeight()) );

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
            ImGui::Image(static_cast<ImTextureID>(naiko->getApplication()->getImguiTextureId(*pbo)), ImVec2(imageWidth, imageHeight), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::EndChild();

        }
        ImGui::End();
    }

    void GameWindow::onNesClock(const NesOnClockEvent& event)
    {

        constexpr const uint16_t PARTICLES_AMOUNT = 1000;

        auto pbo = naiko->getApplication()->getNesGo();

        pbo->setPixels( naiko->getPpu()->getPixels() );

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

            pbo->setPixel(p.p.x, p.p.y, aiko::BLACK);

            p.p.x += p.v.x;
            p.p.y += p.v.y;

            if (p.p.x < 0 || p.p.x >= pbo->getWidth())
            {
                p.v.x = -p.v.x;
                p.p.x += p.v.x;
            }
            if (p.p.y < 0 || p.p.y >= pbo->getHeight())
            {
                p.v.y = -p.v.y;
                p.p.y += p.v.y;
            }

            pbo->setPixel(p.p.x, p.p.y, aiko::WHITE);

        }

        pbo->refresh();
    }

}
