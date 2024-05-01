#include "memory.h"
namespace nes
{

    void Memory::reset()
    {
        for (u32 i = 0 ; i < MAX_MEM ; i++)
        {
            data[i] = 0;
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
        Byte index = address & 0xFFFF;
        return read(index);
    }

    void Memory::write(Word address, Word value)
    {
        Byte high = (value >> 8) & 0xFF;
        Byte low = value & 0xFF;
        assert(address < MAX_MEM);
        data[address + 0] = high;
        data[address + 1] = low;
    }

}
