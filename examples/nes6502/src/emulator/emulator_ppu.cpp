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
            aiko::PboTextureComponent* pbo = naiko->getApplication()->getPalette();
            pbo->updatePixels(pixels);
            pbo->refreshPixels();
        }
    }

    void PpuWindow::render()
    {
        if (ImGui::Begin(name.c_str(), &is_open))
        {
            ImGui::Text("Colour Palette");
            ImGui::BeginChild("Palette Color");
            auto* pbo = naiko->getApplication()->getPalette();
            // Get the dimensions of the texture
            constexpr const float size_multiplier = 16;
            ImVec2 textureSize = ImVec2(pbo->getPboTexture().texture.width * size_multiplier, pbo->getPboTexture().texture.height * size_multiplier);
            ImGui::Image((ImTextureID)pbo->getPboTexture().texture.id, textureSize, ImVec2(0, 0), ImVec2(1, 1));
            ImGui::EndChild();
        }
        ImGui::End();
    }

}
