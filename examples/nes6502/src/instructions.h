#pragma once

#include <exception>

#include "nes_types.h"
#include "microprocessor.h"

namespace nes
{

    enum class AddressModes
    {
        Implied,
        Immediate,
        ZeroPage,
        ZeroPageX,
        ZeroPageY,
        Absolute,
        AbsoluteX,
        AbsoluteY,
        Indirect,
        IndirectX,
        IndirectY,
        Relative,
        Inmediate
    };

    enum class Instruction
    {
        adc,
        and,
        asl,
        bcc,
        bcs,
        beq,
        bit,
        bmi,
        bne,
        bpl,
        brk,
        bvc,
        bvs,
        clc,
        cld,
        cli,
        clv,
        cmp,
        cpx,
        cpy,
        dec,
        dex,
        dey,
        eor,
        inc,
        inx,
        iny,
        jmp,
        jsr,
        lda,
        ldx,
        ldy,
        lsr,
        nop,
        ora,
        pha,
        php,
        pla,
        plp,
        rol,
        ror,
        rti,
        rts,
        sbc,
        sec,
        sed,
        sei,
        sta,
        stx,
        sty,
        tax,
        tay,
        tsx,
        txa,
        txs,
        tya,
    };

    static const char* to_string(AddressModes mode)
    {
        switch (mode)
        {
        case AddressModes::Implied:     return"Implied";
        case AddressModes::Immediate:   return"Immediate";
        case AddressModes::ZeroPage:    return"ZeroPage";
        case AddressModes::ZeroPageX:   return"ZeroPageX";
        case AddressModes::ZeroPageY:   return"ZeroPageY";
        case AddressModes::Absolute:    return"Absolute";
        case AddressModes::AbsoluteX:   return"AbsoluteX";
        case AddressModes::AbsoluteY:   return"AbsoluteY";
        case AddressModes::Indirect:    return"Indirect";
        case AddressModes::IndirectX:   return"IndirectX";
        case AddressModes::IndirectY:   return"IndirectY";
        case AddressModes::Relative:    return"Relative";
        default:                        std::exception("Addressing Mode Unknow");
        }
    }

    struct OpCode
    {
        Byte opCode;
        Byte cycles;
        AddressModes mode;
        Instruction instruction;
    };

}
