#include "emulator_cartridge.h"

#include "emulator/emulator.h"
#include "nes_emulator.h"

#include <aiko_includes.h>
#include <imgui.h>

namespace nes
{
    CartridgeWindow::CartridgeWindow(Naiko* n)
        : EmulatorWindow(n, "Cartridge")
    {

    }

    void CartridgeWindow::update()
    {
        static bool updated = false;
        if (updated == false)
        {
            updated = true;
            Cartridge* cart = naiko->getCartridge();
            auto pixels = convertPatternTableToTexture(cart->getCHR());
            auto* pbo = naiko->getApplication()->getPT0();
            pbo->updatePixels(pixels);
            pbo->refreshPixels();
        }

    }

    void CartridgeWindow::render()
    {
        if (ImGui::Begin(name.c_str(), &is_open))
        {
            ImGui::BeginChild("CHR table");
            auto* pbo = naiko->getApplication()->getPT0();
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

    std::vector<aiko::Color> CartridgeWindow::convertPatternTableToTexture(const std::vector<Byte>& patternTable)
    {
        constexpr Word width = 256;
        constexpr Word height = 128;
        std::vector<aiko::Color> textureData(width * height);
        constexpr Byte n_of_tables = 2;

        for (int table = 0; table < n_of_tables; ++table)
        {
            int startX = table * (width / n_of_tables); // Adjust start X position for the second table
            for (int tileY = 0; tileY < 16; ++tileY)
            {
                for (int tileX = 0; tileX < 16; ++tileX)
                {
                    for (int row = 0; row < 8; ++row)
                    {
                        int baseIndex = table * 4096 + tileY * width + tileX * 16 + row;
                        Byte plane0 = patternTable[baseIndex];
                        Byte plane1 = patternTable[baseIndex + 8];

                        for (int col = 0; col < 8; ++col)
                        {
                            Byte pixelValue = ((plane1 >> (7 - col)) & 1) << 1 | ((plane0 >> (7 - col)) & 1);
                            float color = static_cast<float>(pixelValue); // Normalize to range [0, 1]
                            int texIndex = (startX + tileX * 8 + col) + (tileY * 8 + row) * width; // Adjust index calculation
                            textureData[texIndex] = { color, color, color, 1.0f };
                        }
                    }
                }
            }
        }

        return textureData;
    }

}
