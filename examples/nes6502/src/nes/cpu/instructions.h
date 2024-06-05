#pragma once

#include <exception>
#include <assert.h>
#include <vector>

#include <aiko_types.h>

#include "nes/nes_types.h"
#include "nes/microprocessor.h"

#include <magic_enum.hpp>

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
        auto str = magic_enum::enum_name<AddressModes>(mode);
        return str.data();
    }

    static const char* to_string(Instruction mode)
    {
        auto str = magic_enum::enum_name<Instruction>(mode);
        return str.data();
    }

    struct OpCode
    {
        aiko::string name;
        Instruction instruction;
        AddressModes mode;
        Byte cycles;
    };

    static std::vector<OpCode> instruction_tables =
    {
        { "BRK", Instruction::brk, AddressModes::Immediate, 7 },{ "ORA", Instruction::ora, AddressModes::IndirectX, 6 },{ "???", Instruction::xxx, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 8 },{ "???", Instruction::nop, AddressModes::Implied  , 3 },{ "ORA", Instruction::ora, AddressModes::ZeroPage , 3 },{ "ASL", Instruction::asl, AddressModes::ZeroPage , 5 },{ "???", Instruction::xxx, AddressModes::Implied, 5 },{ "PHP", Instruction::php, AddressModes::Implied, 3 },{ "ORA", Instruction::ora, AddressModes::Immediate, 2 },{ "ASL", Instruction::asl, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 2 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "ORA", Instruction::ora, AddressModes::Absolute , 4 },{ "ASL", Instruction::asl, AddressModes::Absolute , 6 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },
        { "BPL", Instruction::bpl, AddressModes::Relative , 2 },{ "ORA", Instruction::ora, AddressModes::IndirectY, 5 },{ "???", Instruction::xxx, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 8 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "ORA", Instruction::ora, AddressModes::ZeroPageX, 4 },{ "ASL", Instruction::asl, AddressModes::ZeroPageX, 6 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },{ "CLC", Instruction::clc, AddressModes::Implied, 2 },{ "ORA", Instruction::ora, AddressModes::AbsoluteY, 4 },{ "???", Instruction::nop, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 7 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "ORA", Instruction::ora, AddressModes::AbsoluteX, 4 },{ "ASL", Instruction::asl, AddressModes::AbsoluteX, 7 },{ "???", Instruction::xxx, AddressModes::Implied, 7 },
        { "JSR", Instruction::jsr, AddressModes::Absolute , 6 },{ "AND", Instruction::and, AddressModes::IndirectX, 6 },{ "???", Instruction::xxx, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 8 },{ "BIT", Instruction::bit, AddressModes::ZeroPage , 3 },{ "AND", Instruction::and, AddressModes::ZeroPage , 3 },{ "ROL", Instruction::rol, AddressModes::ZeroPage , 5 },{ "???", Instruction::xxx, AddressModes::Implied, 5 },{ "PLP", Instruction::plp, AddressModes::Implied, 4 },{ "AND", Instruction::and, AddressModes::Immediate, 2 },{ "ROL", Instruction::rol, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 2 },{ "BIT", Instruction::bit, AddressModes::Absolute , 4 },{ "AND", Instruction::and, AddressModes::Absolute , 4 },{ "ROL", Instruction::rol, AddressModes::Absolute , 6 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },
        { "BMI", Instruction::bmi, AddressModes::Relative , 2 },{ "AND", Instruction::and, AddressModes::IndirectY, 5 },{ "???", Instruction::xxx, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 8 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "AND", Instruction::and, AddressModes::ZeroPageX, 4 },{ "ROL", Instruction::rol, AddressModes::ZeroPageX, 6 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },{ "SEC", Instruction::sec, AddressModes::Implied, 2 },{ "AND", Instruction::and, AddressModes::AbsoluteY, 4 },{ "???", Instruction::nop, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 7 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "AND", Instruction::and, AddressModes::AbsoluteX, 4 },{ "ROL", Instruction::rol, AddressModes::AbsoluteX, 7 },{ "???", Instruction::xxx, AddressModes::Implied, 7 },
        { "RTI", Instruction::rti, AddressModes::Implied  , 6 },{ "EOR", Instruction::eor, AddressModes::IndirectX, 6 },{ "???", Instruction::xxx, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 8 },{ "???", Instruction::nop, AddressModes::Implied  , 3 },{ "EOR", Instruction::eor, AddressModes::ZeroPage , 3 },{ "LSR", Instruction::lsr, AddressModes::ZeroPage , 5 },{ "???", Instruction::xxx, AddressModes::Implied, 5 },{ "PHA", Instruction::pha, AddressModes::Implied, 3 },{ "EOR", Instruction::eor, AddressModes::Immediate, 2 },{ "LSR", Instruction::lsr, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 2 },{ "JMP", Instruction::jmp, AddressModes::Absolute , 3 },{ "EOR", Instruction::eor, AddressModes::Absolute , 4 },{ "LSR", Instruction::lsr, AddressModes::Absolute , 6 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },
        { "BVC", Instruction::bvc, AddressModes::Relative , 2 },{ "EOR", Instruction::eor, AddressModes::IndirectY, 5 },{ "???", Instruction::xxx, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 8 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "EOR", Instruction::eor, AddressModes::ZeroPageX, 4 },{ "LSR", Instruction::lsr, AddressModes::ZeroPageX, 6 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },{ "CLI", Instruction::cli, AddressModes::Implied, 2 },{ "EOR", Instruction::eor, AddressModes::AbsoluteY, 4 },{ "???", Instruction::nop, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 7 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "EOR", Instruction::eor, AddressModes::AbsoluteX, 4 },{ "LSR", Instruction::lsr, AddressModes::AbsoluteX, 7 },{ "???", Instruction::xxx, AddressModes::Implied, 7 },
        { "RTS", Instruction::rts, AddressModes::Implied  , 6 },{ "ADC", Instruction::adc, AddressModes::IndirectX, 6 },{ "???", Instruction::xxx, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 8 },{ "???", Instruction::nop, AddressModes::Implied  , 3 },{ "ADC", Instruction::adc, AddressModes::ZeroPage , 3 },{ "ROR", Instruction::ror, AddressModes::ZeroPage , 5 },{ "???", Instruction::xxx, AddressModes::Implied, 5 },{ "PLA", Instruction::pla, AddressModes::Implied, 4 },{ "ADC", Instruction::adc, AddressModes::Immediate, 2 },{ "ROR", Instruction::ror, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 2 },{ "JMP", Instruction::jmp, AddressModes::Indirect , 5 },{ "ADC", Instruction::adc, AddressModes::Absolute , 4 },{ "ROR", Instruction::ror, AddressModes::Absolute , 6 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },
        { "BVS", Instruction::bvs, AddressModes::Relative , 2 },{ "ADC", Instruction::adc, AddressModes::IndirectY, 5 },{ "???", Instruction::xxx, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 8 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "ADC", Instruction::adc, AddressModes::ZeroPageX, 4 },{ "ROR", Instruction::ror, AddressModes::ZeroPageX, 6 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },{ "SEI", Instruction::sei, AddressModes::Implied, 2 },{ "ADC", Instruction::adc, AddressModes::AbsoluteY, 4 },{ "???", Instruction::nop, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 7 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "ADC", Instruction::adc, AddressModes::AbsoluteX, 4 },{ "ROR", Instruction::ror, AddressModes::AbsoluteX, 7 },{ "???", Instruction::xxx, AddressModes::Implied, 7 },
        { "???", Instruction::nop, AddressModes::Implied  , 2 },{ "STA", Instruction::sta, AddressModes::IndirectX, 6 },{ "???", Instruction::nop, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },{ "STY", Instruction::sty, AddressModes::ZeroPage , 3 },{ "STA", Instruction::sta, AddressModes::ZeroPage , 3 },{ "STX", Instruction::stx, AddressModes::ZeroPage , 3 },{ "???", Instruction::xxx, AddressModes::Implied, 3 },{ "DEY", Instruction::dey, AddressModes::Implied, 2 },{ "???", Instruction::nop, AddressModes::Implied  , 2 },{ "TXA", Instruction::txa, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 2 },{ "STY", Instruction::sty, AddressModes::Absolute , 4 },{ "STA", Instruction::sta, AddressModes::Absolute , 4 },{ "STX", Instruction::stx, AddressModes::Absolute , 4 },{ "???", Instruction::xxx, AddressModes::Implied, 4 },
        { "BCC", Instruction::bcc, AddressModes::Relative , 2 },{ "STA", Instruction::sta, AddressModes::IndirectY, 6 },{ "???", Instruction::xxx, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },{ "STY", Instruction::sty, AddressModes::ZeroPageX, 4 },{ "STA", Instruction::sta, AddressModes::ZeroPageX, 4 },{ "STX", Instruction::stx, AddressModes::ZeroPageY, 4 },{ "???", Instruction::xxx, AddressModes::Implied, 4 },{ "TYA", Instruction::tya, AddressModes::Implied, 2 },{ "STA", Instruction::sta, AddressModes::AbsoluteY, 5 },{ "TXS", Instruction::txs, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 5 },{ "???", Instruction::nop, AddressModes::Implied  , 5 },{ "STA", Instruction::sta, AddressModes::AbsoluteX, 5 },{ "???", Instruction::xxx, AddressModes::Implied  , 5 },{ "???", Instruction::xxx, AddressModes::Implied, 5 },
        { "LDY", Instruction::ldy, AddressModes::Immediate, 2 },{ "LDA", Instruction::lda, AddressModes::IndirectX, 6 },{ "LDX", Instruction::ldx, AddressModes::Immediate, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },{ "LDY", Instruction::ldy, AddressModes::ZeroPage , 3 },{ "LDA", Instruction::lda, AddressModes::ZeroPage , 3 },{ "LDX", Instruction::ldx, AddressModes::ZeroPage , 3 },{ "???", Instruction::xxx, AddressModes::Implied, 3 },{ "TAY", Instruction::tay, AddressModes::Implied, 2 },{ "LDA", Instruction::lda, AddressModes::Immediate, 2 },{ "TAX", Instruction::tax, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 2 },{ "LDY", Instruction::ldy, AddressModes::Absolute , 4 },{ "LDA", Instruction::lda, AddressModes::Absolute , 4 },{ "LDX", Instruction::ldx, AddressModes::Absolute , 4 },{ "???", Instruction::xxx, AddressModes::Implied, 4 },
        { "BCS", Instruction::bcs, AddressModes::Relative , 2 },{ "LDA", Instruction::lda, AddressModes::IndirectY, 5 },{ "???", Instruction::xxx, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 5 },{ "LDY", Instruction::ldy, AddressModes::ZeroPageX, 4 },{ "LDA", Instruction::lda, AddressModes::ZeroPageX, 4 },{ "LDX", Instruction::ldx, AddressModes::ZeroPageY, 4 },{ "???", Instruction::xxx, AddressModes::Implied, 4 },{ "CLV", Instruction::clv, AddressModes::Implied, 2 },{ "LDA", Instruction::lda, AddressModes::AbsoluteY, 4 },{ "TSX", Instruction::tsx, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 4 },{ "LDY", Instruction::ldy, AddressModes::AbsoluteX, 4 },{ "LDA", Instruction::lda, AddressModes::AbsoluteX, 4 },{ "LDX", Instruction::ldx, AddressModes::AbsoluteY, 4 },{ "???", Instruction::xxx, AddressModes::Implied, 4 },
        { "CPY", Instruction::cpy, AddressModes::Immediate, 2 },{ "CMP", Instruction::cmp, AddressModes::IndirectX, 6 },{ "???", Instruction::nop, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 8 },{ "CPY", Instruction::cpy, AddressModes::ZeroPage , 3 },{ "CMP", Instruction::cmp, AddressModes::ZeroPage , 3 },{ "DEC", Instruction::dec, AddressModes::ZeroPage , 5 },{ "???", Instruction::xxx, AddressModes::Implied, 5 },{ "INY", Instruction::iny, AddressModes::Implied, 2 },{ "CMP", Instruction::cmp, AddressModes::Immediate, 2 },{ "DEX", Instruction::dex, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 2 },{ "CPY", Instruction::cpy, AddressModes::Absolute , 4 },{ "CMP", Instruction::cmp, AddressModes::Absolute , 4 },{ "DEC", Instruction::dec, AddressModes::Absolute , 6 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },
        { "BNE", Instruction::bne, AddressModes::Relative , 2 },{ "CMP", Instruction::cmp, AddressModes::IndirectY, 5 },{ "???", Instruction::xxx, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 8 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "CMP", Instruction::cmp, AddressModes::ZeroPageX, 4 },{ "DEC", Instruction::dec, AddressModes::ZeroPageX, 6 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },{ "CLD", Instruction::cld, AddressModes::Implied, 2 },{ "CMP", Instruction::cmp, AddressModes::AbsoluteY, 4 },{ "NOP", Instruction::nop, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 7 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "CMP", Instruction::cmp, AddressModes::AbsoluteX, 4 },{ "DEC", Instruction::dec, AddressModes::AbsoluteX, 7 },{ "???", Instruction::xxx, AddressModes::Implied, 7 },
        { "CPX", Instruction::cpx, AddressModes::Immediate, 2 },{ "SBC", Instruction::sbc, AddressModes::IndirectX, 6 },{ "???", Instruction::nop, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 8 },{ "CPX", Instruction::cpx, AddressModes::ZeroPage , 3 },{ "SBC", Instruction::sbc, AddressModes::ZeroPage , 3 },{ "INC", Instruction::inc, AddressModes::ZeroPage , 5 },{ "???", Instruction::xxx, AddressModes::Implied, 5 },{ "INX", Instruction::inx, AddressModes::Implied, 2 },{ "SBC", Instruction::sbc, AddressModes::Immediate, 2 },{ "NOP", Instruction::nop, AddressModes::Implied, 2 },{ "???", Instruction::sbc, AddressModes::Implied, 2 },{ "CPX", Instruction::cpx, AddressModes::Absolute , 4 },{ "SBC", Instruction::sbc, AddressModes::Absolute , 4 },{ "INC", Instruction::inc, AddressModes::Absolute , 6 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },
        { "BEQ", Instruction::beq, AddressModes::Relative , 2 },{ "SBC", Instruction::sbc, AddressModes::IndirectY, 5 },{ "???", Instruction::xxx, AddressModes::Implied  , 2 },{ "???", Instruction::xxx, AddressModes::Implied, 8 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "SBC", Instruction::sbc, AddressModes::ZeroPageX, 4 },{ "INC", Instruction::inc, AddressModes::ZeroPageX, 6 },{ "???", Instruction::xxx, AddressModes::Implied, 6 },{ "SED", Instruction::sed, AddressModes::Implied, 2 },{ "SBC", Instruction::sbc, AddressModes::AbsoluteY, 4 },{ "NOP", Instruction::nop, AddressModes::Implied, 2 },{ "???", Instruction::xxx, AddressModes::Implied, 7 },{ "???", Instruction::nop, AddressModes::Implied  , 4 },{ "SBC", Instruction::sbc, AddressModes::AbsoluteX, 4 },{ "INC", Instruction::inc, AddressModes::AbsoluteX, 7 },{ "???", Instruction::xxx, AddressModes::Implied, 7 },
    };

}
