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
        waitForCycles = 0;
    }

    void Cpu::clock()
    {
        if (waitForCycles > 0)
        {
            waitForCycles--;
            return;
        }

        auto memory = bus->getMicroprocesor<Memory>();

        Byte opCode = memory->read( program_counter );
        program_counter++;

        execute(opCode);

    }

    AddressModes Cpu::currentAddressMode() const
    {
        return m_currentAddressMode;
    }

}
