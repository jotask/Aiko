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

    Byte Cpu::fetchByte()
    {
        Memory* mem = bus->getMicroprocesor<Memory>();
        Byte data;// = mem[program_counter];
        program_counter++;
        return data;
    }

    void Cpu::execute(Byte opCode)
    {

    }

}
