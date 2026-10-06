#include "emulator_ppu.h"

#include "emulator/emulator.h"
#include "nes_emulator.h"

#include <aiko_includes.h>

#include "nes/ppu/colour_palette.h"

#include <imgui.h>

namespace nes
{
    PpuWindow::PpuWindow(Naiko* n)
        : EmulatorWindow(n, "Ppu")
    {

    }

    void PpuWindow::update()
    {
        static bool first = true;
        if (first)
        {
            first = false;
            auto pixels = std::vector<aiko::Color>();
            pixels.insert(pixels.end(), &palette::colour_palette[0], &palette::colour_palette[COLOUR_PALETTE_SIZE]);
            auto pbo = naiko->getApplication()->getPalette();
            pbo->setPixels(pixels);
            pbo->refresh();
        }
    }

    void PpuWindow::render()
    {
        if (ImGui::Begin(name.c_str(), &is_open))
        {
            ImGui::Text("Colour Palette");
            ImGui::BeginChild("Palette Color");
            auto pbo = naiko->getApplication()->getPalette();
            // Get the dimensions of the texture
            constexpr const float size_multiplier = 16;
            ImVec2 textureSize = ImVec2(static_cast<float>(pbo->getWidth()) * size_multiplier, static_cast<float>(pbo->getHeight()) * size_multiplier);
            ImGui::Image( static_cast<ImTextureID>(naiko->getApplication()->getImguiTextureId(*pbo)), textureSize, ImVec2(0, 0), ImVec2(1, 1));
            ImGui::EndChild();
        }
        ImGui::End();
    }

}
