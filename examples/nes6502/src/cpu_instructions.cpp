#include "cpu.h"

#include "bus.h"
#include "memory.h"
#include "nes_utils.h"

#include <aiko_includes.h>

namespace nes
{

    void Cpu::adc()
    {
        setCurrentInstruction(Instruction::adc);
        fetchData();
        Word address = A + memoryFetched + getFlag(C);
        setFlag(C, address > 255);
        setFlag(Z, (address & 0x00FF) == 0);
        setFlag(V, (~((Word)A ^ (Word)memoryFetched) & ((Word)A ^ (Word)address)) & 0x0080);
        setFlag(N, address & 0x80);
        A = address & 0x00FF;
        // TODO
    }

    void Cpu:: and()
    {
        setCurrentInstruction(Instruction:: and);
        fetchData();
        A = A & memoryFetched;
        setFlag(Z, A == 0x00);
        setFlag(N, A & 0x80);
        // TODO
    }

    void Cpu::asl()
    {
        setCurrentInstruction(Instruction::asl);
        fetchData();
        Word temp = (Word)memoryFetched << 1;
        setFlag(C, (temp & 0xFF00) > 0);
        setFlag(Z, (temp & 0x00FF) == 0x00);
        setFlag(N, temp & 0x80);
        if (m_currentAddressMode == AddressModes::Implied)
        {
            A = temp & 0x00FF;
        }
        else
        {
            getMemory()->write(addr_abs, Byte(temp & 0x00FF));
        }
    }

    void Cpu::bcc()
    {
        setCurrentInstruction(Instruction::bcc);
        if (getFlag(C) == 0)
        {
            waitForCycles++;
            addr_abs = program_counter + addr_rel;
            if ((addr_abs & 0xFF00) != (program_counter & 0xFF00))
            {
                waitForCycles++;
            }
            program_counter = addr_abs;
        }
    }

    void Cpu::bcs()
    {
        setCurrentInstruction(Instruction::bcs);
        if (getFlag(C) == 1)
        {
            waitForCycles++;
            addr_abs = program_counter + addr_rel;
            if ((addr_abs & 0xFF00) != (waitForCycles & 0xFF00))
            {
                waitForCycles++;
            }
            program_counter = addr_abs;
        }
    }

    void Cpu::beq()
    {
        setCurrentInstruction(Instruction::beq);
        if (getFlag(Z) == 1)
        {
            waitForCycles++;
            addr_abs = program_counter + addr_rel;
            if ((addr_abs & 0xFF00) != (program_counter & 0xFF00))
            {
                waitForCycles++;
            }
            program_counter = addr_abs;
        }
    }

    void Cpu::bit()
    {
        setCurrentInstruction(Instruction::bit);
        fetchData();
        Word temp = A & memoryFetched;
        setFlag(Z, (temp & 0x00FF) == 0x00);
        setFlag(N, memoryFetched & (1 << 7));
        setFlag(V, memoryFetched & (1 << 6));
    }

    void Cpu::bmi()
    {
        setCurrentInstruction(Instruction::bmi);
        if (getFlag(N) == 1)
        {
            waitForCycles++;
            addr_abs = program_counter + addr_rel;
            if ((addr_abs & 0xFF00) != (program_counter & 0xFF00))
            {
                waitForCycles++;
            }
            program_counter = addr_abs;
        }
    }

    void Cpu::bne()
    {
        setCurrentInstruction(Instruction::bne);
        if (getFlag(Z) == 0)
        {
            waitForCycles++;
            addr_abs = program_counter + addr_rel;
            if ((addr_abs & 0xFF00) != (program_counter & 0xFF00))
            {
                waitForCycles++;
            }
            program_counter = addr_abs;
        }
    }

    void Cpu::bpl()
    {
        setCurrentInstruction(Instruction::bpl);
        if (getFlag(N) == 0)
        {
            waitForCycles++;
            addr_abs = program_counter + addr_rel;
            if ((addr_abs & 0xFF00) != (program_counter & 0xFF00))
            {
                waitForCycles++;
            }
            program_counter = addr_abs;
        }
    }

    void Cpu::brk()
    {
        setCurrentInstruction(Instruction::brk);
        program_counter++;
        setFlag(I, 1);
        pushWordStack(program_counter);
        pushWordStack(program_counter, true);
        setFlag(B, 1);
        pushStack(P, true);
        setFlag(B, 0);
        Byte high = getMemory()->read(Word(0xFFFE));
        Byte low  = getMemory()->read(Word(0xFFFF));
        program_counter = toWord(high, low);
    }

    void Cpu::bvc()
    {
        setCurrentInstruction(Instruction::bvc);
        if (getFlag(V) == false)
        {
            waitForCycles++;
            addr_abs = program_counter + addr_rel;
            if ((addr_abs & 0xFF00) != (program_counter & 0xFF00))
            {
                waitForCycles++;
            }
            program_counter = addr_abs;
        }
    }

    void Cpu::bvs()
    {
        setCurrentInstruction(Instruction::bvs);
        if (getFlag(V) == 1)
        {
            waitForCycles++;
            addr_abs = program_counter + addr_rel;
            if ((addr_abs & 0xFF00) != (program_counter & 0xFF00))
            {
                waitForCycles++;
            }
            program_counter = addr_abs;
        }
    }

    void Cpu::clc()
    {
        setCurrentInstruction(Instruction::clc);
        setFlag(C, false);
    }

    void Cpu::cld()
    {
        setCurrentInstruction(Instruction::cld);
        setFlag(D, false);
    }

    void Cpu::cli()
    {
        setCurrentInstruction(Instruction::cli);
        setFlag(I, false);
    }

    void Cpu::clv()
    {
        setCurrentInstruction(Instruction::clv);
        setFlag(V, false);
    }

    void Cpu::cmp()
    {
        setCurrentInstruction(Instruction::cmp);
        fetchData();
        Word temp = (Word)A - (Word)memoryFetched;
        setFlag(C, A >= memoryFetched);
        setFlag(Z, (temp & 0x00FF) == 0x0000);
        setFlag(N, temp & 0x0080);
    }

    void Cpu::cpx()
    {
        setCurrentInstruction(Instruction::cpx);
        fetchData();
        Word temp = (Word)X - (Word)memoryFetched;
        setFlag(C, X >= memoryFetched);
        setFlag(Z, (temp & 0x00FF) == 0x0000);
        setFlag(N, temp & 0x0080);
    }

    void Cpu::cpy()
    {
        setCurrentInstruction(Instruction::cpy);
        fetchData();
        Word temp = (Word)Y - (Word)memoryFetched;
        setFlag(C, Y >= memoryFetched);
        setFlag(Z, (temp & 0x00FF) == 0x0000);
        setFlag(N, temp & 0x0080);
    }

    void Cpu::dec()
    {
        setCurrentInstruction(Instruction::dec);
        fetchData();
        Word temp = memoryFetched - 1;
        getMemory()->write(addr_abs, Byte(temp & 0x00FF));
        setFlag(Z, (temp & 0x00FF) == 0x0000);
        setFlag(N, temp & 0x0080);
    }

    void Cpu::dex()
    {
        setCurrentInstruction(Instruction::dex);
        X--;
        setFlag(Z, X == 0x00);
        setFlag(N, X & 0x80);
    }

    void Cpu::dey()
    {
        setCurrentInstruction(Instruction::dey);
        Y--;
        setFlag(Z, Y == 0x00);
        setFlag(N, Y & 0x80);
    }

    void Cpu::eor()
    {
        setCurrentInstruction(Instruction::eor);
        fetchData();
        A = A ^ memoryFetched;
        setFlag(Z, A == 0x00);
        setFlag(N, A & 0x80);
    }

    void Cpu::inc()
    {
        setCurrentInstruction(Instruction::inc);
        fetchData();
        Word temp = memoryFetched + 1;
        getMemory()->write(addr_abs, Byte(temp & 0x00FF));
        setFlag(Z, (temp & 0x00FF) == 0x0000);
        setFlag(N, temp & 0x0080);
    }

    void Cpu::inx()
    {
        setCurrentInstruction(Instruction::inx);
        X++;
        setFlag(Z, X == 0x00);
        setFlag(N, X & 0x80);
    }

    void Cpu::iny()
    {
        setCurrentInstruction(Instruction::iny);
        Y++;
        setFlag(Z, Y == 0x00);
        setFlag(N, Y & 0x80);
    }

    void Cpu::jmp()
    {
        setCurrentInstruction(Instruction::jmp);
        program_counter = addr_abs;
    }

    void Cpu::jsr()
    {
        setCurrentInstruction(Instruction::jsr);
        program_counter--;
        pushWordStack(program_counter, true);
        program_counter = addr_abs;
    }

    void Cpu::lda()
    {
        setCurrentInstruction(Instruction::lda);
        fetchData();
        A = memoryFetched;
        setFlag(Z, A == 0x00);
        setFlag(N, A & 0x80);
    }

    void Cpu::ldx()
    {
        setCurrentInstruction(Instruction::ldx);
        fetchData();
        X = memoryFetched;
        setFlag(Z, X == 0x00);
        setFlag(N, X & 0x80);
    }

    void Cpu::ldy()
    {
        setCurrentInstruction(Instruction::ldy);
        fetchData();
        Y = memoryFetched;
        setFlag(Z, Y == 0x00);
        setFlag(N, Y & 0x80);
    }

    void Cpu::lsr()
    {
        setCurrentInstruction(Instruction::lsr);
        fetchData();
        setFlag(C, memoryFetched & 0x0001);
        Word temp = memoryFetched >> 1;
        setFlag(Z, (temp & 0x00FF) == 0x0000);
        setFlag(N, temp & 0x0080);
        if (m_currentAddressMode == AddressModes::Implied)
        {
            A = temp & 0x00FF;
        }
        else
        {
            getMemory()->write(addr_abs, Byte(temp & 0x00FF));
        }
    }

    void Cpu::nop()
    {
        setCurrentInstruction(Instruction::nop);
    }

    void Cpu::ora()
    {
        setCurrentInstruction(Instruction::ora);
        fetchData();
        A = A | memoryFetched;
        setFlag(Z, A == 0x00);
        setFlag(N, A & 0x80);
    }

    void Cpu::pha()
    {
        setCurrentInstruction(Instruction::pha);
        pushStack(A, true);
    }

    void Cpu::php()
    {
        setCurrentInstruction(Instruction::php);
        pushStack(Byte(P | B | U), true);
        setFlag(B, 0);
        setFlag(U, 0);
    }

    void Cpu::pla()
    {
        setCurrentInstruction(Instruction::pla);
        A = popStack(false);
        setFlag(Z, A == 0x00);
        setFlag(N, A & 0x80);
    }

    void Cpu::plp()
    {
        setCurrentInstruction(Instruction::plp);
        bool prevB = getFlag(B);
        bool prevU = getFlag(U);
        P = popStack(false);
        setFlag(B, prevB);
        setFlag(U, prevU);
    }

    void Cpu::rol()
    {
        setCurrentInstruction(Instruction::rol);
        fetchData();
        Word temp = (Word)(memoryFetched << 1) | getFlag(C);
        setFlag(C, temp & 0xFF00);
        setFlag(Z, (temp & 0x00FF) == 0x0000);
        setFlag(N, temp & 0x0080);
        if (m_currentAddressMode == AddressModes::Implied)
        {
            A = temp & 0x00FF;
        }
        else
        {
            getMemory()->write(addr_abs, Byte(temp & 0x00FF));
        }
    }

    void Cpu::ror()
    {
        setCurrentInstruction(Instruction::ror);
        fetchData();
        Word temp = (Word)(getFlag(C) << 7) | (memoryFetched >> 1);
        setFlag(C, memoryFetched & 0x01);
        setFlag(Z, (temp & 0x00FF) == 0x00);
        setFlag(N, temp & 0x0080);
        if (m_currentAddressMode == AddressModes::Implied)
        {
            A = temp & 0x00FF;
        }
        else
        {
            getMemory()->write(addr_abs, Byte(temp & 0x00FF));
        }
    }

    void Cpu::rti()
    {
        setCurrentInstruction(Instruction::rti);
        bool prevB = getFlag(B);
        bool prevU = getFlag(U);
        P = popStack(false);
        setFlag(B, prevB);
        setFlag(U, prevU);
        program_counter = popWordStack();
    }

    void Cpu::rts()
    {
        setCurrentInstruction(Instruction::rts);
        program_counter = popWordStack();
        program_counter++;
    }

    void Cpu::sbc()
    {
        setCurrentInstruction(Instruction::sbc);
        fetchData();
        uint16_t value = ((Word)memoryFetched) ^ 0x00FF;
        Word temp = (Word)A + value + (Word)getFlag(C);
        setFlag(C, temp & 0xFF00);
        setFlag(Z, ((temp & 0x00FF) == 0));
        setFlag(V, (temp ^ (Word)A) & (temp ^ value) & 0x0080);
        setFlag(N, temp & 0x0080);
        A = temp & 0x00FF;
        // TODO
    }

    void Cpu::sec()
    {
        setCurrentInstruction(Instruction::sec);
        setFlag(C, true);
    }

    void Cpu::sed()
    {
        setCurrentInstruction(Instruction::sed);
        setFlag(D, true);
    }

    void Cpu::sei()
    {
        setCurrentInstruction(Instruction::sei);
        setFlag(I, true);
    }

    void Cpu::sta()
    {
        setCurrentInstruction(Instruction::sta);
        getMemory()->write(addr_abs, A);
    }

    void Cpu::stx()
    {
        setCurrentInstruction(Instruction::stx);
        getMemory()->write(addr_abs, X);
    }

    void Cpu::sty()
    {
        setCurrentInstruction(Instruction::sty);
        getMemory()->write(addr_abs, Y);
    }

    void Cpu::tax()
    {
        setCurrentInstruction(Instruction::tax);
        X = A;
        setFlag(Z, X == 0x00);
        setFlag(N, X & 0x80);
    }

    void Cpu::tay()
    {
        setCurrentInstruction(Instruction::tay);
        Y = A;
        setFlag(Z, Y == 0x00);
        setFlag(N, Y & 0x80);
    }
    
    void Cpu::txa()
    {
        setCurrentInstruction(Instruction::txa);
        A = X;
        setFlag(Z, A == 0x00);
        setFlag(N, A & 0x80);
    }

    void Cpu::tya()
    {
        setCurrentInstruction(Instruction::tya);
        A = Y;
        setFlag(Z, A == 0x00);
        setFlag(N, A & 0x80);
    }

    void Cpu::tsx()
    {
        setCurrentInstruction(Instruction::tsx);
        X = stack_pointer;
        setFlag(Z, X == 0x00);
        setFlag(N, X & 0x80);
    }

    void Cpu::txs()
    {
        setCurrentInstruction(Instruction::txs);
        stack_pointer = X;
    }

    void Cpu::xxx()
    {
        setCurrentInstruction(Instruction::xxx);
    }

}
