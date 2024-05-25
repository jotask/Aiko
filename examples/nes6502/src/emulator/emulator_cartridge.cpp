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
            ImVec2 wsize = ImVec2(pbo->getPboTexture().texture.width, pbo->getPboTexture().texture.height);
            ImGui::Image((ImTextureID)(intptr_t)pbo->getPboTexture().texture.id, wsize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::EndChild();
        }
        ImGui::End();
    }

    std::vector<aiko::Color> CartridgeWindow::convertPatternTableToTexture(const std::vector<Byte>& patternTable)
    {
        constexpr Word width = 128;
        constexpr Word height = 256;
        std::vector<aiko::Color> textureData(width * height);

        for (int table = 0; table < 2; ++table)
        {
            for (int tileY = 0; tileY < 16; ++tileY)
            {
                for (int tileX = 0; tileX < 16; ++tileX)
                {
                    for (int row = 0; row < 8; ++row)
                    {

                        int baseIndex = table * 4096 + tileY * 256 + tileX * 16 + row;
                        Byte plane0 = patternTable[baseIndex];
                        Byte plane1 = patternTable[baseIndex + 8];

                        for (int col = 0; col < 8; ++col)
                        {
                            Byte pixelValue = ((plane1 >> (7 - col)) & 1) << 1 | ((plane0 >> (7 - col)) & 1);
                            float color = static_cast<float>(pixelValue);
                            int texIndex = (table * 128 * 128) + (tileY * 8 + row) * 128 + (tileX * 8 + col);
                            textureData[texIndex] = { color, color, color, 1.0f };
                        }
                    }
                }
            }
        }

        return textureData;
    }

}
