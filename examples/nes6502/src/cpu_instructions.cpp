#include "cpu.h"

#include "bus.h"
#include "memory.h"
#include "nes_utils.h"

#include <aiko_includes.h>

namespace nes
{

    void Cpu::adc()
    {
        m_currentInstruction = Instruction::adc;
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
        m_currentInstruction = Instruction:: and;
        A = A & memoryFetched;
        setFlag(Z, A == 0x00);
        setFlag(N, A & 0x80);
        // TODO
    }

    void Cpu::asl()
    {
        m_currentInstruction = Instruction::asl;
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
        m_currentInstruction = Instruction::bcc;
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
        m_currentInstruction = Instruction::bcs;
        if (getFlag(C) == 1)
        {
            waitForCycles++;
            addr_abs = program_counter + addr_rel;
            if ((addr_abs & 0xFF00) != (waitForCycles & 0xFF00))
            {
                waitForCycles++;
            }
            waitForCycles = addr_abs;
        }
    }

    void Cpu::beq()
    {
        m_currentInstruction = Instruction::beq;
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
        m_currentInstruction = Instruction::bit;
        Word temp = A & memoryFetched;
        setFlag(Z, (temp & 0x00FF) == 0x00);
        setFlag(N, memoryFetched & (1 << 7));
        setFlag(V, memoryFetched & (1 << 6));
    }

    void Cpu::bmi()
    {
        m_currentInstruction = Instruction::bmi;
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
        m_currentInstruction = Instruction::bne;
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
        m_currentInstruction = Instruction::bpl;
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

        // TODO extract this into the push/pop stack functions

        program_counter++;
        setFlag(I, 1);
        Memory* mem = getMemory();
        mem->write(Word(0x0100 + stack_pointer), Byte((program_counter >> 8) & 0x00FF));
        stack_pointer--;
        mem->write(Word(0x0100 + stack_pointer), Byte(program_counter & 0x00FF));
        stack_pointer--;

        setFlag(B, 1);
        mem->write(Word(0x0100 + stack_pointer), P);
        stack_pointer--;
        setFlag(B, 0);

        program_counter = (Word)mem->read(Word(0xFFFE)) | ((Word)mem->read(Word(0xFFFF)) << 8);
    }

    void Cpu::bvc()
    {
        m_currentInstruction = Instruction::bvc;
        if (getFlag(V) == 0)
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
        m_currentInstruction = Instruction::bvs;
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
        m_currentInstruction = Instruction::clc;
        setFlag(C, false);
    }

    void Cpu::cld()
    {
        m_currentInstruction = Instruction::cld;
        setFlag(D, false);
    }

    void Cpu::cli()
    {
        m_currentInstruction = Instruction::cli;
        setFlag(I, false);
    }

    void Cpu::clv()
    {
        m_currentInstruction = Instruction::clv;
        setFlag(V, false);
    }

    void Cpu::cmp()
    {
        m_currentInstruction = Instruction::cmp;
        Word temp = (Word)A - (Word)memoryFetched;
        setFlag(C, A >= memoryFetched);
        setFlag(Z, (temp & 0x00FF) == 0x0000);
        setFlag(N, temp & 0x0080);
    }

    void Cpu::cpx()
    {
        m_currentInstruction = Instruction::cpx;
        Word temp = (Word)X - (Word)memoryFetched;
        setFlag(C, X >= memoryFetched);
        setFlag(Z, (temp & 0x00FF) == 0x0000);
        setFlag(N, temp & 0x0080);
    }

    void Cpu::cpy()
    {
        m_currentInstruction = Instruction::cpy;
        Word temp = (Word)Y - (Word)memoryFetched;
        setFlag(C, Y >= memoryFetched);
        setFlag(Z, (temp & 0x00FF) == 0x0000);
        setFlag(N, temp & 0x0080);
    }

    void Cpu::dec()
    {
        m_currentInstruction = Instruction::dec;
        Word temp = memoryFetched - 1;
        getMemory()->write(addr_abs, Byte(temp & 0x00FF));
        setFlag(Z, (temp & 0x00FF) == 0x0000);
        setFlag(N, temp & 0x0080);
    }

    void Cpu::dex()
    {
        m_currentInstruction = Instruction::dex;
        X--;
        setFlag(Z, X == 0x00);
        setFlag(N, X & 0x80);
    }

    void Cpu::dey()
    {
        m_currentInstruction = Instruction::dey;
        Y--;
        setFlag(Z, Y == 0x00);
        setFlag(N, Y & 0x80);
    }

    void Cpu::eor()
    {
        m_currentInstruction = Instruction::eor;
        A = A ^ memoryFetched;
        setFlag(Z, A == 0x00);
        setFlag(N, A & 0x80);
    }

    void Cpu::inc()
    {
        m_currentInstruction = Instruction::inc;
        Word temp = memoryFetched + 1;
        getMemory()->write(addr_abs, Byte(temp & 0x00FF));
        setFlag(Z, (temp & 0x00FF) == 0x0000);
        setFlag(N, temp & 0x0080);
    }

    void Cpu::inx()
    {
        m_currentInstruction = Instruction::inx;
        X++;
        setFlag(Z, X == 0x00);
        setFlag(N, X & 0x80);
    }

    void Cpu::iny()
    {
        m_currentInstruction = Instruction::iny;
        Y++;
        setFlag(Z, Y == 0x00);
        setFlag(N, Y & 0x80);
    }

    void Cpu::jmp()
    {
        m_currentInstruction = Instruction::jmp;
        program_counter = addr_abs;
    }

    void Cpu::jsr()
    {
        m_currentInstruction = Instruction::jsr;
        program_counter--;

        getMemory()->write(Word(0x0100 + stack_pointer), Byte((program_counter >> 8) & 0x00FF));
        stack_pointer--;
        getMemory()->write(Word(0x0100 + stack_pointer), Byte(program_counter & 0x00FF));
        stack_pointer--;

        program_counter = addr_abs;
    }

    void Cpu::lda()
    {
        m_currentInstruction = Instruction::lda;
        A = memoryFetched;
        setFlag(Z, A == 0x00);
        setFlag(N, A & 0x80);
    }

    void Cpu::ldx()
    {
        m_currentInstruction = Instruction::ldx;
        X = memoryFetched;
        setFlag(Z, X == 0x00);
        setFlag(N, X & 0x80);
    }

    void Cpu::ldy()
    {
        m_currentInstruction = Instruction::ldy;
        Y = memoryFetched;
        setFlag(Z, Y == 0x00);
        setFlag(N, Y & 0x80);
    }

    void Cpu::lsr()
    {
        m_currentInstruction = Instruction::lsr;
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
        m_currentInstruction = Instruction::nop;
        // TODO
    }

    void Cpu::ora()
    {
        m_currentInstruction = Instruction::ora;
        A = A | memoryFetched;
        setFlag(Z, A == 0x00);
        setFlag(N, A & 0x80);
    }

    void Cpu::pha()
    {
        m_currentInstruction = Instruction::pha;
        getMemory()->write(Word(0x0100 + stack_pointer), A);
        stack_pointer--;
    }

    void Cpu::php()
    {
        m_currentInstruction = Instruction::php;
        getMemory()->write(Word(0x0100 + stack_pointer), Byte(P | B | U));
        setFlag(B, 0);
        setFlag(U, 0);
        stack_pointer--;
    }

    void Cpu::pla()
    {
        m_currentInstruction = Instruction::pla;
        stack_pointer++;
        A = getMemory()->read(Word(0x0100 + stack_pointer));
        setFlag(Z, A == 0x00);
        setFlag(N, A & 0x80);
    }

    void Cpu::plp()
    {
        m_currentInstruction = Instruction::plp;
        stack_pointer++;
        P = getMemory()->read(Word(0x0100 + stack_pointer));
        setFlag(U, 1);
    }

    void Cpu::rol()
    {
        m_currentInstruction = Instruction::rol;
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
        m_currentInstruction = Instruction::ror;
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
        m_currentInstruction = Instruction::rti;
        stack_pointer++;
        P = getMemory()->read(Word(0x0100 + stack_pointer));
        P &= ~B;
        P &= ~U;

        stack_pointer++;
        program_counter = (Word)getMemory()->read(Word(0x0100 + stack_pointer));
        stack_pointer++;
        program_counter |= (Word)getMemory()->read(Word((0x0100 + stack_pointer) << 8));
    }

    void Cpu::rts()
    {
        m_currentInstruction = Instruction::rts;
        stack_pointer++;
        program_counter = (Word)getMemory()->read(Word(0x0100 + stack_pointer));
        stack_pointer++;
        program_counter |= (Word)getMemory()->read(Word((0x0100 + stack_pointer) << 8));

        program_counter++;
    }

    void Cpu::sbc()
    {
        m_currentInstruction = Instruction::sbc;
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
        m_currentInstruction = Instruction::sec;
        setFlag(C, true);
    }

    void Cpu::sed()
    {
        m_currentInstruction = Instruction::sed;
        setFlag(D, true);
    }

    void Cpu::sei()
    {
        m_currentInstruction = Instruction::sei;
        setFlag(I, true);
    }

    void Cpu::sta()
    {
        m_currentInstruction = Instruction::sta;
        getMemory()->write(addr_abs, A);
    }

    void Cpu::stx()
    {
        m_currentInstruction = Instruction::stx;
        getMemory()->write(addr_abs, X);
    }

    void Cpu::sty()
    {
        m_currentInstruction = Instruction::sty;
        getMemory()->write(addr_abs, Y);
    }

    void Cpu::tax()
    {
        m_currentInstruction = Instruction::tax;
        X = A;
        setFlag(Z, X == 0x00);
        setFlag(N, X & 0x80);
    }

    void Cpu::tay()
    {
        m_currentInstruction = Instruction::tay;
        Y = A;
        setFlag(Z, Y == 0x00);
        setFlag(N, Y & 0x80);
    }
    
    void Cpu::txa()
    {
        m_currentInstruction = Instruction::txa;
        A = X;
        setFlag(Z, A == 0x00);
        setFlag(N, A & 0x80);
    }

    void Cpu::tya()
    {
        m_currentInstruction = Instruction::tya;
        A = Y;
        setFlag(Z, A == 0x00);
        setFlag(N, A & 0x80);
    }

    void Cpu::tsx()
    {
        m_currentInstruction = Instruction::tsx;
        X = stack_pointer;
        setFlag(Z, X == 0x00);
        setFlag(N, X & 0x80);
    }

    void Cpu::txs()
    {
        m_currentInstruction = Instruction::txs;
        stack_pointer = X;
    }

    void Cpu::xxx()
    {
        m_currentInstruction = Instruction::xxx;
    }


}
