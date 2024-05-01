#include "cpu.h"

#include "bus.h"
#include "memory.h"

namespace nes
{

    void Cpu::reset()
    {
        program_counter = 0xFFFC;
        stack_pointer = 0x0100;
        C = Z = I = D = B = V = N = 0;
        A = X = Y = 0;
    }

    void Cpu::clock()
    {

    }

        program_counter++;
    }

}
