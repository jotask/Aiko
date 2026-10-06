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
            naiko->getApplication()->updatePaletteTexture(pixels);
        }
    }

    void PpuWindow::render()
    {
        if (ImGui::Begin(name.c_str(), &is_open))
        {
            ImGui::Text("Colour Palette");
            ImGui::BeginChild("Palette Color");

            constexpr float sizeMultiplier = 16.0f;
            constexpr float paletteWidth = static_cast<float>(COLOUR_PALETTE_SIZE / 4);
            constexpr float paletteHeight = static_cast<float>(COLOUR_PALETTE_SIZE / 16);

            const ImVec2 textureSize { paletteWidth * sizeMultiplier, paletteHeight * sizeMultiplier };

            NesEmulator* app = naiko->getApplication();

            ImGui::Image(static_cast<ImTextureID>( app->getImguiTextureId( app->getPaletteTextureId() ) ), textureSize, ImVec2(0, 0), ImVec2(1, 1));

            ImGui::EndChild();
        }
        ImGui::End();
    }

}
