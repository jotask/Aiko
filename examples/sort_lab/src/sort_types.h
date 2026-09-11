#pragma once

#include <cstdint>
#include <array>

namespace sb
{

    #define UPDATE_STEP_TIME 0.01f

    using NUMBER = uint16_t;
    constexpr uint16_t N_NUMBERS = 64;

    using Numeros = std::array<NUMBER, N_NUMBERS>;

}
