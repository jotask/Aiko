#include "cpu.h"

#include "bus.h"
#include "memory.h"
#include "nes_utils.h"

#include "aiko_includes.h"

namespace nes
{

    void Cpu::execute(Byte opCode)
    {
        Byte& cycles = this->waitForCycles;
        switch (opCode)
        {
            // adc
        case 0x69:      cycles = 2;     inmediate();        adc();      break;
        case 0x65:      cycles = 3;     zeroPage();         adc();      break;
        case 0x75:      cycles = 4;     zeroPageX();        adc();      break;
        case 0x6d:      cycles = 4;     absolute();         adc();      break;
        case 0x7d:      cycles = 4;     absoluteX();        adc();      break;
        case 0x79:      cycles = 4;     absoluteY();        adc();      break;
        case 0x61:      cycles = 6;     indirectX();        adc();      break;
        case 0x71:      cycles = 5;     indirectY();        adc();      break;
            // lda
        case 0xa9:      cycles = 2;     inmediate();        lda();      break;
        case 0xa5:      cycles = 3;     zeroPage();         lda();      break;
        case 0xb5:      cycles = 4;     zeroPageX();        lda();      break;
        case 0xad:      cycles = 4;     absolute();         lda();      break;
        case 0xbd:      cycles = 4;     absoluteX();        lda();      break;
        case 0xb9:      cycles = 4;     absoluteY();        lda();      break;
        case 0xa1:      cycles = 6;     indirectX();        lda();      break;
        case 0xb1:      cycles = 5;     indirectY();        lda();      break;
            // ldx
        case 0xa2:      cycles = 2;     inmediate();        ldx();      break;
        case 0xa6:      cycles = 3;     zeroPage();         ldx();      break;
        case 0xb6:      cycles = 4;     zeroPageY();        ldx();      break;
        case 0xae:      cycles = 4;     absolute();         ldx();      break;
        case 0xbe:      cycles = 4;     absoluteY();        ldx();      break;
        default:
            aiko::Log::warning("OpCode Not Implemented", toString(opCode));
            break;
        }

        assert(this->waitForCycles == cycles);

    }

}
