#pragma once

#include <string>
#include <vector>
#include <tuple>

#include "nes_types.h"
#include "nes_utils.h"

namespace nes::test::online
{

    struct CpuState
    {
        nes::Word pc;
        nes::Word s;
        nes::Byte a;
        nes::Byte x;
        nes::Byte y;
        nes::Byte p;
        std::vector<std::tuple<nes::Word, nes::Byte>> ram;
    };

    using CycleState = std::vector<std::tuple<nes::Word, nes::Byte, std::string>>;

}
