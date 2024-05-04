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
            // and
        case 0x29:      cycles = 2;     inmediate();        and();      break;
        case 0x25:      cycles = 3;     zeroPage();         and();      break;
        case 0x35:      cycles = 4;     zeroPageX();        and();      break;
        case 0x2d:      cycles = 4;     absolute();         and();      break;
        case 0x3d:      cycles = 4;     absoluteX();        and();      break;
        case 0x39:      cycles = 4;     absoluteY();        and();      break;
        case 0x21:      cycles = 6;     indirectX();        and();      break;
        case 0x31:      cycles = 5;     indirectY();        and();      break;
            // asl
        case 0x0a:      cycles = 2;     accumulator();      asl();      break;
        case 0x06:      cycles = 5;     zeroPage();         asl();      break;
        case 0x16:      cycles = 6;     zeroPageX();        asl();      break;
        case 0x0e:      cycles = 6;     absolute();         asl();      break;
        case 0x1e:      cycles = 7;     absoluteX();        asl();      break;
            // bcc
        case 0x90:      cycles = 2;     relative();         bcc();      break;
            // bcs
        case 0xb0:      cycles = 2;     relative();         bcs();      break;
            // beq
        case 0xf0:      cycles = 2;     relative();         beq();      break;
            // bit
        case 0x24:      cycles = 3;     zeroPage();         bit();      break;
        case 0x2c:      cycles = 4;     absolute();         bit();      break;
            // bmi
        case 0x30:      cycles = 2;     relative();         bmi();      break;
            // bne
        case 0xd0:      cycles = 2;     relative();         bne();      break;
            // bpl
        case 0x10:      cycles = 2;     relative();         bpl();      break;
            // brk
        case 0x00:      cycles = 7;     implied();          brk();      break;
            // bvc
        case 0x50:      cycles = 2;     relative();         bvc();      break;
            // bvs
        case 0x70:      cycles = 2;     relative();         bvs();      break;
            // clc
        case 0x18:      cycles = 2;     relative();         clc();      break;
            // cld
        case 0xd8:      cycles = 2;     implied();          cld();      break;
            // cli
        case 0x58:      cycles = 2;     implied();          cli();      break;
            // clv
        case 0xb8:      cycles = 2;     implied();          clv();      break;
            // cmp
        case 0xc9:      cycles = 2;     inmediate();        cmp();      break;
        case 0xc5:      cycles = 3;     zeroPage();         cmp();      break;
        case 0xd5:      cycles = 4;     zeroPageX();        cmp();      break;
        case 0xcd:      cycles = 4;     absolute();         cmp();      break;
        case 0xdd:      cycles = 4;     absoluteX();        cmp();      break;
        case 0xd9:      cycles = 4;     absoluteY();        cmp();      break;
        case 0xc1:      cycles = 6;     indirectX();        cmp();      break;
        case 0xd1:      cycles = 5;     indirectY();        cmp();      break;
            // cpx
        case 0xe0:      cycles = 2;     inmediate();        cpx();      break;
        case 0xe4:      cycles = 3;     zeroPage();         cpx();      break;
        case 0xec:      cycles = 4;     absolute();         cpx();      break;
            // cpy
        case 0xc0:      cycles = 2;     inmediate();        cpy();      break;
        case 0xc4:      cycles = 3;     zeroPage();         cpy();      break;
        case 0xcc:      cycles = 4;     absolute();         cpy();      break;
            // dec
        case 0xc6:      cycles = 5;     zeroPage();         dec();      break;
        case 0xd6:      cycles = 6;     zeroPageX();        dec();      break;
        case 0xce:      cycles = 6;     absolute();         dec();      break;
        case 0xde:      cycles = 7;     absoluteX();        dec();      break;
            // dex
        case 0xca:      cycles = 2;     implied();          dex();      break;
            // dey
        case 0x88:      cycles = 2;     implied();          dey();      break;
            // eor
        case 0x49:      cycles = 2;     inmediate();        eor();      break;
        case 0x45:      cycles = 3;     zeroPage();         eor();      break;
        case 0x55:      cycles = 4;     zeroPageX();        eor();      break;
        case 0x4d:      cycles = 4;     absolute();         eor();      break;
        case 0x5d:      cycles = 4;     absoluteX();        eor();      break;
        case 0x59:      cycles = 4;     absoluteY();        eor();      break;
        case 0x41:      cycles = 6;     indirectX();        eor();      break;
        case 0x51:      cycles = 5;     indirectY();        eor();      break;
            // inc
        case 0xe6:      cycles = 5;     zeroPage();         inc();      break;
        case 0xf6:      cycles = 6;     zeroPageX();        inc();      break;
        case 0xee:      cycles = 6;     absolute();         inc();      break;
        case 0xfe:      cycles = 7;     absoluteX();        inc();      break;
            // inx
        case 0xe8:      cycles = 2;     implied();          inx();      break;
            // iny
        case 0xc8:      cycles = 2;     implied();          iny();      break;
            // jmp
        case 0x4c:      cycles = 3;     absolute();         jmp();      break;
        case 0x6c:      cycles = 5;     indirect();         jmp();      break;
            // jsr
        case 0x20:      cycles = 3;     absolute();         jsr();      break;
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
            // ldy
        case 0xa0:      cycles = 2;     inmediate();        ldy();      break;
        case 0xa4:      cycles = 3;     zeroPage();         ldy();      break;
        case 0xb4:      cycles = 4;     zeroPageX();        ldy();      break;
        case 0xac:      cycles = 4;     absolute();         ldy();      break;
        case 0xbc:      cycles = 4;     absoluteX();        ldy();      break;
            // lsr
        case 0x4a:      cycles = 2;     accumulator();      lsr();      break;
        case 0x46:      cycles = 5;     zeroPage();         lsr();      break;
        case 0x56:      cycles = 6;     zeroPageX();        lsr();      break;
        case 0x4e:      cycles = 6;     absolute();         lsr();      break;
        case 0x5e:      cycles = 7;     absoluteX();        lsr();      break;
            // nop
        case 0xea:      cycles = 2;     implied();          nop();      break;
            // ora
        case 0x09:      cycles = 2;     inmediate();        ora();      break;
        case 0x05:      cycles = 3;     zeroPage();         ora();      break;
        case 0x15:      cycles = 4;     zeroPageX();        ora();      break;
        case 0x0d:      cycles = 4;     absolute();         ora();      break;
        case 0x1d:      cycles = 4;     absoluteX();        ora();      break;
        case 0x19:      cycles = 4;     absoluteY();        ora();      break;
        case 0x01:      cycles = 6;     indirectX();        ora();      break;
        case 0x11:      cycles = 5;     indirectY();        ora();      break;
            // pha
        case 0x48:      cycles = 3;     implied();          pha();      break;
            // php
        case 0x08:      cycles = 3;     implied();          php();      break;
            // pla
        case 0x68:      cycles = 4;     implied();          pla();      break;
            // plp
        case 0x28:      cycles = 4;     implied();          plp();      break;
            // rol
        case 0x2a:      cycles = 2;     accumulator();      rol();      break;
        case 0x26:      cycles = 5;     zeroPage();         rol();      break;
        case 0x36:      cycles = 6;     zeroPageX();        rol();      break;
        case 0x2e:      cycles = 6;     absolute();         rol();      break;
        case 0x3e:      cycles = 7;     absoluteX();        rol();      break;
            // ror
        case 0x6a:      cycles = 2;     accumulator();      ror();      break;
        case 0x66:      cycles = 5;     zeroPage();         ror();      break;
        case 0x76:      cycles = 6;     zeroPageX();        ror();      break;
        case 0x6e:      cycles = 6;     absolute();         ror();      break;
        case 0x7e:      cycles = 7;     absoluteX();        ror();      break;
            // rti
        case 0x40:      cycles = 6;     implied();          rti();      break;
            // rts
        case 0x60:      cycles = 6;     implied();          rts();      break;
            // sbc
        case 0xe9:      cycles = 2;     inmediate();        sbc();      break;
        case 0xe5:      cycles = 3;     zeroPage();         sbc();      break;
        case 0xf5:      cycles = 4;     zeroPageX();        sbc();      break;
        case 0xed:      cycles = 4;     absolute();         sbc();      break;
        case 0xfd:      cycles = 4;     absoluteX();        sbc();      break;
        case 0xf9:      cycles = 4;     absoluteY();        sbc();      break;
        case 0xe1:      cycles = 6;     indirectX();        sbc();      break;
        case 0xf1:      cycles = 5;     indirectY();        sbc();      break;
            // sec
        case 0x38:      cycles = 2;     implied();          sec();      break;
            // sed
        case 0xf8:      cycles = 2;     implied();          sed();      break;
            // sei
        case 0x78:      cycles = 2;     implied();          sei();      break;
            // sta
        case 0x85:      cycles = 3;     zeroPage();         sta();      break;
        case 0x95:      cycles = 4;     zeroPageX();        sta();      break;
        case 0x8d:      cycles = 4;     absolute();         sta();      break;
        case 0x9d:      cycles = 5;     absoluteX();        sta();      break;
        case 0x99:      cycles = 5;     absoluteY();        sta();      break;
        case 0x81:      cycles = 6;     indirectX();        sta();      break;
        case 0x91:      cycles = 6;     indirectY();        sta();      break;
            // stx
        case 0x86:      cycles = 3;     zeroPage();         stx();      break;
        case 0x96:      cycles = 4;     zeroPageY();        stx();      break;
        case 0x8e:      cycles = 4;     absolute();         stx();      break;
            // sty
        case 0x84:      cycles = 3;     zeroPage();         sty();      break;
        case 0x94:      cycles = 4;     zeroPageX();        sty();      break;
        case 0x8c:      cycles = 4;     absolute();         sty();      break;
            // tax
        case 0xaa:      cycles = 2;     implied();         tax();       break;
            // tay
        case 0xa8:      cycles = 2;     implied();         tay();       break;
            // tsx
        case 0xba:      cycles = 2;     implied();         tsx();       break;
            // txa
        case 0x8a:      cycles = 2;     implied();         txa();       break;
            // txs
        case 0x9a:      cycles = 2;     implied();         txs();       break;
            // tya
        case 0x98:      cycles = 2;     implied();         tya();       break;
        default:
            aiko::Log::warning("OpCode Not Implemented ", toString(opCode));
            break;
        }

        assert(this->waitForCycles == cycles);

    }

}
