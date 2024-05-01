#include "cpu.h"

#include "bus.h"
#include "memory.h"

namespace nes
{


    void Cpu::execute(Byte opCode)
    {
        switch (opCode)
        {
        // adc
        case 0x69:      inmediate();        adc();  break;
        case 0x65:      zeroPage();         adc();  break;
        case 0x75:      zeroPageX();        adc();  break;
        case 0x6d:      absolute();         adc();  break;
        case 0x7d:      absoluteX();        adc();  break;
        case 0x79:      absoluteY();        adc();  break;
        case 0x61:      indirectX();        adc();  break;
        case 0x71:      indirectY();        adc();  break;
        default:
            break;
        }
    }

}
