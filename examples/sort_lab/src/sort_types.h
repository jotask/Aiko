#pragma once

#include <cstdint>
#include <array>

namespace sb
{

    #define UPDATE_STEP_TIME 0.01f

    using NUMBER = uint16_t;
    constexpr uint16_t MAX_VALUE = 32;

    using Numeros = std::array<NUMBER, MAX_VALUE>;

}
