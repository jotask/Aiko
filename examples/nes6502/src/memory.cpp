#include "memory.h"

#include "nes_utils.h"

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
        assert(address < MAX_MEM);
        data[address + 0] = getHigh(value);
        data[address + 1] = getLow(value);
    }

}
