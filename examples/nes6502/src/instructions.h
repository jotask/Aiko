#pragma once

#include <exception>

#include "nes_types.h"
#include "microprocessor.h"

namespace nes
{

    enum class AddressModes
    {
        Implied,
        Inmediate,
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

    static const char* to_string(Instruction mode)
    {
        switch (mode)
        {
            case Instruction::adc:      return "adc";
            case Instruction::and:      return "and";
            case Instruction::asl:      return "asl";
            case Instruction::bcc:      return "bcc";
            case Instruction::bcs:      return "bcs";
            case Instruction::beq:      return "beq";
            case Instruction::bit:      return "bit";
            case Instruction::bmi:      return "bmi";
            case Instruction::bne:      return "bne";
            case Instruction::bpl:      return "bpl";
            case Instruction::brk:      return "brk";
            case Instruction::bvc:      return "bvc";
            case Instruction::bvs:      return "bvs";
            case Instruction::clc:      return "clc";
            case Instruction::cld:      return "cld";
            case Instruction::cli:      return "cli";
            case Instruction::clv:      return "clv";
            case Instruction::cmp:      return "cmp";
            case Instruction::cpx:      return "cpx";
            case Instruction::cpy:      return "cpy";
            case Instruction::dec:      return "dec";
            case Instruction::dex:      return "dex";
            case Instruction::dey:      return "dey";
            case Instruction::eor:      return "eor";
            case Instruction::inc:      return "inc";
            case Instruction::inx:      return "inx";
            case Instruction::iny:      return "iny";
            case Instruction::jmp:      return "jmp";
            case Instruction::jsr:      return "jsr";
            case Instruction::lda:      return "lda";
            case Instruction::ldx:      return "ldx";
            case Instruction::ldy:      return "ldy";
            case Instruction::lsr:      return "lsr";
            case Instruction::nop:      return "nop";
            case Instruction::ora:      return "ora";
            case Instruction::pha:      return "pha";
            case Instruction::php:      return "php";
            case Instruction::pla:      return "pla";
            case Instruction::plp:      return "plp";
            case Instruction::rol:      return "rol";
            case Instruction::ror:      return "ror";
            case Instruction::rti:      return "rti";
            case Instruction::rts:      return "rts";
            case Instruction::sbc:      return "sbc";
            case Instruction::sec:      return "sec";
            case Instruction::sed:      return "sed";
            case Instruction::sei:      return "sei";
            case Instruction::sta:      return "sta";
            case Instruction::stx:      return "stx";
            case Instruction::sty:      return "sty";
            case Instruction::tax:      return "tax";
            case Instruction::tay:      return "tay";
            case Instruction::tsx:      return "tsx";
            case Instruction::txa:      return "txa";
            case Instruction::txs:      return "txs";
            case Instruction::tya:      return "tya";
            default:                    std::exception("Addressing Mode Unknow");
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
