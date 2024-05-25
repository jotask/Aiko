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
            ImGui::BeginChild("Palette Color");
            auto* pbo = naiko->getApplication()->getPalette();
            // Get the dimensions of the texture
            ImVec2 textureSize = ImVec2(pbo->getPboTexture().texture.width, pbo->getPboTexture().texture.height);

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

            ImGui::Image((ImTextureID)pbo->getPboTexture().texture.id, ImVec2(imageWidth, imageHeight), ImVec2(0, 0), ImVec2(1, 1));
            ImGui::EndChild();
        }
        ImGui::End();
    }

}
