#pragma once

#include <exception>
#include <assert.h>
#include <string>
#include <vector>

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
        xxx,
    };

    static const char* to_string(AddressModes mode)
    {
        switch (mode)
        {
        case AddressModes::Implied:     return "Implied";
        case AddressModes::Inmediate:   return "Immediate";
        case AddressModes::ZeroPage:    return "ZeroPage";
        case AddressModes::ZeroPageX:   return "ZeroPageX";
        case AddressModes::ZeroPageY:   return "ZeroPageY";
        case AddressModes::Absolute:    return "Absolute";
        case AddressModes::AbsoluteX:   return "AbsoluteX";
        case AddressModes::AbsoluteY:   return "AbsoluteY";
        case AddressModes::Indirect:    return "Indirect";
        case AddressModes::IndirectX:   return "IndirectX";
        case AddressModes::IndirectY:   return "IndirectY";
        case AddressModes::Relative:    return "Relative";
        default:                        return "xxx";       // assert(false, "Addressing Mode Unknow");
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
            case Instruction::xxx:      return "xxx";
            default:                    return "???"; //assert(false, "Instruction Unknow");
        }
    }

    struct OpCode
    {
        std::string name;
        Instruction instruction;
        AddressModes mode;
        Byte cycles;
    };

    static std::vector<OpCode> instruction_tables =
    {
        { "BRK", Instruction::brk, AddressModes::Inmediate, 7 },{ "ORA", Instruction::ora, AddressModes::IndirectX, 6 },{ "???", Instruction::xxx, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 8 },{ "???", Instruction::nop, AddressModes::Implied  , 3 },{ "ORA", Instruction::ora, AddressModes::ZeroPage , 3 },{ "ASL", Instruction::asl, AddressModes::ZeroPage , 5 },{ "???", Instruction::xxx, AddressModes::Implied, 5 },{ "PHP", Instruction::php, AddressModes::Implied, 3 },{ "ORA", Instruction::ora, AddressModes::Inmediate, 2 },{ "ASL", Instruction::asl, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 2 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "ORA", Instruction::ora, AddressModes::Absolute , 4 },{ "ASL", Instruction::asl, AddressModes::Absolute , 6 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },
        { "BPL", Instruction::bpl, AddressModes::Relative , 2 },{ "ORA", Instruction::ora, AddressModes::IndirectY, 5 },{ "???", Instruction::xxx, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 8 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "ORA", Instruction::ora, AddressModes::ZeroPageX, 4 },{ "ASL", Instruction::asl, AddressModes::ZeroPageX, 6 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },{ "CLC", Instruction::clc, AddressModes::Implied, 2 },{ "ORA", Instruction::ora, AddressModes::AbsoluteY, 4 },{ "???", Instruction::nop, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 7 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "ORA", Instruction::ora, AddressModes::AbsoluteX, 4 },{ "ASL", Instruction::asl, AddressModes::AbsoluteX, 7 },{ "???", Instruction::xxx, AddressModes::Implied, 7 },
        { "JSR", Instruction::jsr, AddressModes::Absolute , 6 },{ "AND", Instruction::and, AddressModes::IndirectX, 6 },{ "???", Instruction::xxx, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 8 },{ "BIT", Instruction::bit, AddressModes::ZeroPage , 3 },{ "AND", Instruction::and, AddressModes::ZeroPage , 3 },{ "ROL", Instruction::rol, AddressModes::ZeroPage , 5 },{ "???", Instruction::xxx, AddressModes::Implied, 5 },{ "PLP", Instruction::plp, AddressModes::Implied, 4 },{ "AND", Instruction::and, AddressModes::Inmediate, 2 },{ "ROL", Instruction::rol, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 2 },{ "BIT", Instruction::bit, AddressModes::Absolute , 4 },{ "AND", Instruction::and, AddressModes::Absolute , 4 },{ "ROL", Instruction::rol, AddressModes::Absolute , 6 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },
        { "BMI", Instruction::bmi, AddressModes::Relative , 2 },{ "AND", Instruction::and, AddressModes::IndirectY, 5 },{ "???", Instruction::xxx, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 8 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "AND", Instruction::and, AddressModes::ZeroPageX, 4 },{ "ROL", Instruction::rol, AddressModes::ZeroPageX, 6 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },{ "SEC", Instruction::sec, AddressModes::Implied, 2 },{ "AND", Instruction::and, AddressModes::AbsoluteY, 4 },{ "???", Instruction::nop, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 7 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "AND", Instruction::and, AddressModes::AbsoluteX, 4 },{ "ROL", Instruction::rol, AddressModes::AbsoluteX, 7 },{ "???", Instruction::xxx, AddressModes::Implied, 7 },
        { "RTI", Instruction::rti, AddressModes::Implied  , 6 },{ "EOR", Instruction::eor, AddressModes::IndirectX, 6 },{ "???", Instruction::xxx, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 8 },{ "???", Instruction::nop, AddressModes::Implied  , 3 },{ "EOR", Instruction::eor, AddressModes::ZeroPage , 3 },{ "LSR", Instruction::lsr, AddressModes::ZeroPage , 5 },{ "???", Instruction::xxx, AddressModes::Implied, 5 },{ "PHA", Instruction::pha, AddressModes::Implied, 3 },{ "EOR", Instruction::eor, AddressModes::Inmediate, 2 },{ "LSR", Instruction::lsr, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 2 },{ "JMP", Instruction::jmp, AddressModes::Absolute , 3 },{ "EOR", Instruction::eor, AddressModes::Absolute , 4 },{ "LSR", Instruction::lsr, AddressModes::Absolute , 6 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },
        { "BVC", Instruction::bvc, AddressModes::Relative , 2 },{ "EOR", Instruction::eor, AddressModes::IndirectY, 5 },{ "???", Instruction::xxx, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 8 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "EOR", Instruction::eor, AddressModes::ZeroPageX, 4 },{ "LSR", Instruction::lsr, AddressModes::ZeroPageX, 6 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },{ "CLI", Instruction::cli, AddressModes::Implied, 2 },{ "EOR", Instruction::eor, AddressModes::AbsoluteY, 4 },{ "???", Instruction::nop, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 7 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "EOR", Instruction::eor, AddressModes::AbsoluteX, 4 },{ "LSR", Instruction::lsr, AddressModes::AbsoluteX, 7 },{ "???", Instruction::xxx, AddressModes::Implied, 7 },
        { "RTS", Instruction::rts, AddressModes::Implied  , 6 },{ "ADC", Instruction::adc, AddressModes::IndirectX, 6 },{ "???", Instruction::xxx, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 8 },{ "???", Instruction::nop, AddressModes::Implied  , 3 },{ "ADC", Instruction::adc, AddressModes::ZeroPage , 3 },{ "ROR", Instruction::ror, AddressModes::ZeroPage , 5 },{ "???", Instruction::xxx, AddressModes::Implied, 5 },{ "PLA", Instruction::pla, AddressModes::Implied, 4 },{ "ADC", Instruction::adc, AddressModes::Inmediate, 2 },{ "ROR", Instruction::ror, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 2 },{ "JMP", Instruction::jmp, AddressModes::Indirect , 5 },{ "ADC", Instruction::adc, AddressModes::Absolute , 4 },{ "ROR", Instruction::ror, AddressModes::Absolute , 6 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },
        { "BVS", Instruction::bvs, AddressModes::Relative , 2 },{ "ADC", Instruction::adc, AddressModes::IndirectY, 5 },{ "???", Instruction::xxx, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 8 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "ADC", Instruction::adc, AddressModes::ZeroPageX, 4 },{ "ROR", Instruction::ror, AddressModes::ZeroPageX, 6 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },{ "SEI", Instruction::sei, AddressModes::Implied, 2 },{ "ADC", Instruction::adc, AddressModes::AbsoluteY, 4 },{ "???", Instruction::nop, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 7 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "ADC", Instruction::adc, AddressModes::AbsoluteX, 4 },{ "ROR", Instruction::ror, AddressModes::AbsoluteX, 7 },{ "???", Instruction::xxx, AddressModes::Implied, 7 },
        { "???", Instruction::nop, AddressModes::Implied  , 2 },{ "STA", Instruction::sta, AddressModes::IndirectX, 6 },{ "???", Instruction::nop, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },{ "STY", Instruction::sty, AddressModes::ZeroPage , 3 },{ "STA", Instruction::sta, AddressModes::ZeroPage , 3 },{ "STX", Instruction::stx, AddressModes::ZeroPage , 3 },{ "???", Instruction::xxx, AddressModes::Implied, 3 },{ "DEY", Instruction::dey, AddressModes::Implied, 2 },{ "???", Instruction::nop, AddressModes::Implied  , 2 },{ "TXA", Instruction::txa, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 2 },{ "STY", Instruction::sty, AddressModes::Absolute , 4 },{ "STA", Instruction::sta, AddressModes::Absolute , 4 },{ "STX", Instruction::stx, AddressModes::Absolute , 4 },{ "???", Instruction::xxx, AddressModes::Implied, 4 },
        { "BCC", Instruction::bcc, AddressModes::Relative , 2 },{ "STA", Instruction::sta, AddressModes::IndirectY, 6 },{ "???", Instruction::xxx, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },{ "STY", Instruction::sty, AddressModes::ZeroPageX, 4 },{ "STA", Instruction::sta, AddressModes::ZeroPageX, 4 },{ "STX", Instruction::stx, AddressModes::ZeroPageY, 4 },{ "???", Instruction::xxx, AddressModes::Implied, 4 },{ "TYA", Instruction::tya, AddressModes::Implied, 2 },{ "STA", Instruction::sta, AddressModes::AbsoluteY, 5 },{ "TXS", Instruction::txs, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 5 },{ "???", Instruction::nop, AddressModes::Implied  , 5 },{ "STA", Instruction::sta, AddressModes::AbsoluteX, 5 },{ "???", Instruction::xxx, AddressModes::Implied  , 5 },{ "???", Instruction::xxx, AddressModes::Implied, 5 },
        { "LDY", Instruction::ldy, AddressModes::Inmediate, 2 },{ "LDA", Instruction::lda, AddressModes::IndirectX, 6 },{ "LDX", Instruction::ldx, AddressModes::Inmediate, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },{ "LDY", Instruction::ldy, AddressModes::ZeroPage , 3 },{ "LDA", Instruction::lda, AddressModes::ZeroPage , 3 },{ "LDX", Instruction::ldx, AddressModes::ZeroPage , 3 },{ "???", Instruction::xxx, AddressModes::Implied, 3 },{ "TAY", Instruction::tay, AddressModes::Implied, 2 },{ "LDA", Instruction::lda, AddressModes::Inmediate, 2 },{ "TAX", Instruction::tax, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 2 },{ "LDY", Instruction::ldy, AddressModes::Absolute , 4 },{ "LDA", Instruction::lda, AddressModes::Absolute , 4 },{ "LDX", Instruction::ldx, AddressModes::Absolute , 4 },{ "???", Instruction::xxx, AddressModes::Implied, 4 },
        { "BCS", Instruction::bcs, AddressModes::Relative , 2 },{ "LDA", Instruction::lda, AddressModes::IndirectY, 5 },{ "???", Instruction::xxx, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 5 },{ "LDY", Instruction::ldy, AddressModes::ZeroPageX, 4 },{ "LDA", Instruction::lda, AddressModes::ZeroPageX, 4 },{ "LDX", Instruction::ldx, AddressModes::ZeroPageY, 4 },{ "???", Instruction::xxx, AddressModes::Implied, 4 },{ "CLV", Instruction::clv, AddressModes::Implied, 2 },{ "LDA", Instruction::lda, AddressModes::AbsoluteY, 4 },{ "TSX", Instruction::tsx, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 4 },{ "LDY", Instruction::ldy, AddressModes::AbsoluteX, 4 },{ "LDA", Instruction::lda, AddressModes::AbsoluteX, 4 },{ "LDX", Instruction::ldx, AddressModes::AbsoluteY, 4 },{ "???", Instruction::xxx, AddressModes::Implied, 4 },
        { "CPY", Instruction::cpy, AddressModes::Inmediate, 2 },{ "CMP", Instruction::cmp, AddressModes::IndirectX, 6 },{ "???", Instruction::nop, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 8 },{ "CPY", Instruction::cpy, AddressModes::ZeroPage , 3 },{ "CMP", Instruction::cmp, AddressModes::ZeroPage , 3 },{ "DEC", Instruction::dec, AddressModes::ZeroPage , 5 },{ "???", Instruction::xxx, AddressModes::Implied, 5 },{ "INY", Instruction::iny, AddressModes::Implied, 2 },{ "CMP", Instruction::cmp, AddressModes::Inmediate, 2 },{ "DEX", Instruction::dex, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 2 },{ "CPY", Instruction::cpy, AddressModes::Absolute , 4 },{ "CMP", Instruction::cmp, AddressModes::Absolute , 4 },{ "DEC", Instruction::dec, AddressModes::Absolute , 6 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },
        { "BNE", Instruction::bne, AddressModes::Relative , 2 },{ "CMP", Instruction::cmp, AddressModes::IndirectY, 5 },{ "???", Instruction::xxx, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 8 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "CMP", Instruction::cmp, AddressModes::ZeroPageX, 4 },{ "DEC", Instruction::dec, AddressModes::ZeroPageX, 6 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },{ "CLD", Instruction::cld, AddressModes::Implied, 2 },{ "CMP", Instruction::cmp, AddressModes::AbsoluteY, 4 },{ "NOP", Instruction::nop, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 7 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "CMP", Instruction::cmp, AddressModes::AbsoluteX, 4 },{ "DEC", Instruction::dec, AddressModes::AbsoluteX, 7 },{ "???", Instruction::xxx, AddressModes::Implied, 7 },
        { "CPX", Instruction::cpx, AddressModes::Inmediate, 2 },{ "SBC", Instruction::sbc, AddressModes::IndirectX, 6 },{ "???", Instruction::nop, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 8 },{ "CPX", Instruction::cpx, AddressModes::ZeroPage , 3 },{ "SBC", Instruction::sbc, AddressModes::ZeroPage , 3 },{ "INC", Instruction::inc, AddressModes::ZeroPage , 5 },{ "???", Instruction::xxx, AddressModes::Implied, 5 },{ "INX", Instruction::inx, AddressModes::Implied, 2 },{ "SBC", Instruction::sbc, AddressModes::Inmediate, 2 },{ "NOP", Instruction::nop, AddressModes::Implied, 2 },{ "???", Instruction::sbc, AddressModes::Implied, 2 },{ "CPX", Instruction::cpx, AddressModes::Absolute , 4 },{ "SBC", Instruction::sbc, AddressModes::Absolute , 4 },{ "INC", Instruction::inc, AddressModes::Absolute , 6 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },
        { "BEQ", Instruction::beq, AddressModes::Relative , 2 },{ "SBC", Instruction::sbc, AddressModes::IndirectY, 5 },{ "???", Instruction::xxx, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 8 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "SBC", Instruction::sbc, AddressModes::ZeroPageX, 4 },{ "INC", Instruction::inc, AddressModes::ZeroPageX, 6 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },{ "SED", Instruction::sed, AddressModes::Implied, 2 },{ "SBC", Instruction::sbc, AddressModes::AbsoluteY, 4 },{ "NOP", Instruction::nop, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 7 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "SBC", Instruction::sbc, AddressModes::AbsoluteX, 4 },{ "INC", Instruction::inc, AddressModes::AbsoluteX, 7 },{ "???", Instruction::xxx, AddressModes::Implied, 7 },
    };

}
