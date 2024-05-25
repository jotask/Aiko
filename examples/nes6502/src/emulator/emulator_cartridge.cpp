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

    }

    void CartridgeWindow::render()
    {
        if (ImGui::Begin(name.c_str(), &is_open))
        {

            Cartridge* cart = naiko->getCartridge();
            auto pixels = convertPatternTableToTexture(cart->getCHR());

            auto* pbo = naiko->getApplication()->getPT0();
            pbo->updatePixels(pixels);
            pbo->refreshPixels();

            ImGui::BeginChild("CHR table");
            // Get the size of the child (i.e. the whole draw size of the windows).
            ImVec2 wsize = ImGui::GetWindowSize();
            // Because I use the texture from OpenGL, I need to invert the V from the UV.
            ImGui::Image((ImTextureID)pbo->getPboTexture().texture.id, wsize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::EndChild();

        }
        ImGui::End();
    }

    std::vector<aiko::Color> CartridgeWindow::convertPatternTableToTexture(const std::vector<Byte>& patternTable)
    {
        constexpr uint16_t size = 128 * 256;
        std::vector<aiko::Color> textureData(size);

        for (int tileY = 0; tileY < 16; ++tileY)
        {
            for (int tileX = 0; tileX < 16; ++tileX)
            {
                for (int row = 0; row < 8; ++row)
                {

                    Byte plane0 = patternTable[tileY * 256 + tileX * 16 + row];
                    Byte plane1 = patternTable[tileY * 256 + tileX * 16 + row + 8];

                    for (int col = 0; col < 8; ++col) 
                    {

                        Byte pixelValue = ((plane1 >> (7 - col)) & 1) << 1 | ((plane0 >> (7 - col)) & 1);
                        float color = pixelValue * 85; // Simple grayscale based on pixel value
                        int texIndex = (tileY * 8 + row) * 256 + (tileX * 8 + col);
                        textureData[texIndex] = { color, color, color, 1.0f };
                    }
                }
            }
        }

        return textureData;
    }

}
