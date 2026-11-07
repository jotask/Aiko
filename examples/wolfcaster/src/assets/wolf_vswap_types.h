#pragma once

#include <cstdint>
#include <array>
#include <vector>

namespace wc
{

    struct WolfWallTexture
    {
        std::array<uint8_t, 64 * 64> paletteIndices = {0};
    };

    struct WolfSprite
    {
        std::array<uint8_t, 64 * 64> paletteIndices = {0};
        std::array<uint8_t, 64 * 64> alphaMask = {0};
    };

    struct WolfDigitizedSound
    {
        std::vector<uint8_t> samples;
        uint32_t sampleRate = 0;
    };

    struct WolfVSwapData
    {
        std::vector<WolfWallTexture> walls;
        std::vector<WolfSprite> sprites;
        std::vector<WolfDigitizedSound> sounds;
    };

}
