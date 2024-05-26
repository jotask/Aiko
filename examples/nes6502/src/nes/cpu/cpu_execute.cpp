#include "nes/cpu/cpu.h"

#include "nes/bus.h"
#include "nes/memory.h"
#include "nes/utils/nes_utils.h"

#include "aiko_includes.h"
#include "instructions.h"

#include "nes/tests/nes_test_log.h"

namespace nes
{

    void Cpu::execute(Byte opCode)
    {

        Byte& cycles = this->waitForCycles;
        if constexpr (NES_CPU_LOG)
        {
            aiko::Log::trace("Executing [", toString(opCode), "] OpCode");
        }

        OpCode op  = instruction_tables[opCode];

        cycles += op.cycles;

        switch (op.mode)
        {
            case AddressModes::Implied:     implied();      break;
            case AddressModes::Immediate:   immediate();    break;
            case AddressModes::ZeroPage:    zeroPage();     break;
            case AddressModes::ZeroPageX:   zeroPageX();    break;
            case AddressModes::ZeroPageY:   zeroPageY();    break;
            case AddressModes::Absolute:    absolute();     break;
            case AddressModes::AbsoluteX:   absoluteX();    break;
            case AddressModes::AbsoluteY:   absoluteY();    break;
            case AddressModes::Indirect:    indirect();     break;
            case AddressModes::IndirectX:   indirectX();    break;
            case AddressModes::IndirectY:   indirectY();    break;
            case AddressModes::Relative:    relative();     break;
            default:                        assert(false, "unkown addressing mode");
        }

        switch (op.instruction)
        {
            case Instruction::adc:      adc();      break;
            case Instruction::and:      and();      break;
            case Instruction::asl:      asl();      break;
            case Instruction::bcc:      bcc();      break;
            case Instruction::bcs:      bcs();      break;
            case Instruction::beq:      beq();      break;
            case Instruction::bit:      bit();      break;
            case Instruction::bmi:      bmi();      break;
            case Instruction::bne:      bne();      break;
            case Instruction::bpl:      bpl();      break;
            case Instruction::brk:      brk();      break;
            case Instruction::bvc:      bvc();      break;
            case Instruction::bvs:      bvs();      break;
            case Instruction::clc:      clc();      break;
            case Instruction::cld:      cld();      break;
            case Instruction::cli:      cli();      break;
            case Instruction::clv:      clv();      break;
            case Instruction::cmp:      cmp();      break;
            case Instruction::cpx:      cpx();      break;
            case Instruction::cpy:      cpy();      break;
            case Instruction::dec:      dec();      break;
            case Instruction::dex:      dex();      break;
            case Instruction::dey:      dey();      break;
            case Instruction::eor:      eor();      break;
            case Instruction::inc:      inc();      break;
            case Instruction::inx:      inx();      break;
            case Instruction::iny:      iny();      break;
            case Instruction::jmp:      jmp();      break;
            case Instruction::jsr:      jsr();      break;
            case Instruction::lda:      lda();      break;
            case Instruction::ldx:      ldx();      break;
            case Instruction::ldy:      ldy();      break;
            case Instruction::lsr:      lsr();      break;
            case Instruction::nop:      nop();      break;
            case Instruction::ora:      ora();      break;
            case Instruction::pha:      pha();      break;
            case Instruction::php:      php();      break;
            case Instruction::pla:      pla();      break;
            case Instruction::plp:      plp();      break;
            case Instruction::rol:      rol();      break;
            case Instruction::ror:      ror();      break;
            case Instruction::rti:      rti();      break;
            case Instruction::rts:      rts();      break;
            case Instruction::sbc:      sbc();      break;
            case Instruction::sec:      sec();      break;
            case Instruction::sed:      sed();      break;
            case Instruction::sei:      sei();      break;
            case Instruction::sta:      sta();      break;
            case Instruction::stx:      stx();      break;
            case Instruction::sty:      sty();      break;
            case Instruction::tax:      tax();      break;
            case Instruction::tay:      tay();      break;
            case Instruction::tsx:      tsx();      break;
            case Instruction::txa:      txa();      break;
            case Instruction::txs:      txs();      break;
            case Instruction::tya:      tya();      break;
            case Instruction::xxx:      xxx();      break;
            default:                    assert(false, "unkown instruction");
        }

        if constexpr (NES_CPU_LOG)
        {
            aiko::Log::trace("  OpCode: ", toString(opCode), " Addressing Modes: ", to_string(m_currentAddressMode), " Instruction: ", to_string(m_currentInstruction) );
        }

        line++;

        if constexpr (NES_CPU_LOG)
        {
            aiko::Log::info("A:", toString(A), " X: ", toString(X), " Y: ", toString(Y), " P: ", toString(P), " SP: ", toString(stack_pointer), " Line: ", unsigned(line), " PC: ", unsigned(program_counter));
        }
        if constexpr ( NES_TESTS_ENABLED )
        {
            test::NesTest::it().test(line, op, program_counter, stack_pointer, A, X, Y, P);
        }
        assert(this->waitForCycles == cycles);

    }

}
 