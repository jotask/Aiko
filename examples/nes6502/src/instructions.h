#pragma once

#include "nes_types.h"
#include "microprocessor.h"

namespace nes
{

    class Instruction
    {
        Byte opCode;
        u32 cycles;
    };

}
