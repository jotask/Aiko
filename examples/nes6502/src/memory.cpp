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

    Byte Memory::readByte(Byte address)
    {
        assert(address < MAX_MEM);
        return data[address];
    }

    void Memory::writeByte(Byte address, Byte value)
    {
        assert(address < MAX_MEM);
        data[address] = value;
    }

}
