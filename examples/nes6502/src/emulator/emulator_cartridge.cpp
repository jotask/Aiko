#include "emulator_cartridge.h"

#include "emulator/emulator.h"
#include "nes_emulator.h"
#include "nes/ppu/colour_palette.h"

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
            const auto chr = cart->getCHR();
            if (chr.empty())
            {
                return;
            }
            auto pixels = convertPatternTableToTexture(chr);
            naiko->getApplication()->updatePatternTableTexture(pixels);
        }

    }

    void CartridgeWindow::render()
    {
        if (ImGui::Begin(name.c_str(), &is_open))
        {
            ImGui::BeginChild("CHR table");

            constexpr ImVec2 textureSize = { 256.0f, 128.0f };

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
            
            NesEmulator* app = naiko->getApplication();

            const ImTextureID textureId = static_cast<ImTextureID>( app->getImguiTextureId( app->getPatternTableTextureId() ) );

            ImGui::Image(textureId, ImVec2(imageWidth, imageHeight), ImVec2(0, 0), ImVec2(1, 1));
            ImGui::EndChild();
        }
        ImGui::End();
    }

    std::vector<aiko::Color> CartridgeWindow::convertPatternTableToTexture(const std::vector<Byte>& patternTable)
    {
        constexpr Word width = 256;
        constexpr Word height = 128;
        constexpr Byte n_of_tables = 2;
        constexpr Word tableSize = 4096;
        std::vector<aiko::Color> textureData(width * height);

        for (int table = 0; table < n_of_tables; ++table)
        {
            int startX = table * (width / n_of_tables);
            for (int tileY = 0; tileY < 16; ++tileY)
            {
                for (int tileX = 0; tileX < 16; ++tileX)
                {
                    for (int row = 0; row < 8; ++row)
                    {
                        int baseIndex = table * tableSize + (tileY * 16 + tileX) * 16 + row;
                        Byte plane0 = patternTable[baseIndex];
                        Byte plane1 = patternTable[baseIndex + 8];

                        for (int col = 0; col < 8; ++col)
                        {
                            Byte pixelValue = ((plane1 >> (7 - col)) & 1) << 1 | ((plane0 >> (7 - col)) & 1);
                            int texIndex = (startX + tileX * 8 + col) + (tileY * 8 + row) * width;
                            textureData[texIndex] = palette::colour_palette[pixelValue];
                        }
                    }
                }
            }
        }

        return textureData;
    }

}
