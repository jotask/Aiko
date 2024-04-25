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

}
