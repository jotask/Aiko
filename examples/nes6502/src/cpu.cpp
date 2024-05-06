#include "cpu.h"

#include "bus.h"
#include "memory.h"

namespace nes
{

    void Cpu::reset()
    {
        program_counter = 0xFFFC;
        stack_pointer = std::get<0>(Memory::STACK_PAGE);
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

        Byte opCode = memory->read( program_counter++ );

        execute(opCode);

    }

    AddressModes Cpu::currentAddressMode() const
    {
        return m_currentAddressMode;
    }

    Instruction Cpu::currentInstruction() const
    {
        return m_currentInstruction;
    }

    Memory* Cpu::getMemory()
    {
        Memory* mem = bus->getMicroprocesor<Memory>();
        assert(mem != nullptr, "Memory not found in buffer");
        return mem;
    }

}
