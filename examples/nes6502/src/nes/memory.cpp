#include "nes/memory.h"

#include "nes/utils/nes_utils.h"

namespace nes
{

    void Memory::reset()
    {
        for (u32 i = 0; i < MAX_MEM; i++)
        {
            data[i] = 0;
        }
        // It is common practice on a 6502 to initialize the stack pointer to $FF at reset time.
        for (Word i = std::get<0>(STACK_PAGE); i < std::get<1>(STACK_PAGE); i++)
        {
            data[i] = 0xFF;
        }
    }

    Byte Memory::read(Byte address)
    {
        assert(address < MAX_MEM);
        return data[address];
    }

    void Memory::write(Byte address, Byte value)
    {
        assert(address < MAX_MEM);
        data[address] = value;
    }

    Byte Memory::read(Word address)
    {
        assert(address < MAX_MEM);
        return data[address];
    }

    void Memory::write(Word address, Byte value)
    {
        assert(address < MAX_MEM);
        data[address] = value;
    }

    void Memory::write(Word address, Word value)
    {
        data[address + 0] = getHigh(value);
        data[address + 1] = getLow(value);
    }

}
