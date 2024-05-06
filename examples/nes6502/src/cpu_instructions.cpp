#include "cpu.h"

#include "bus.h"
#include "memory.h"
#include "nes_utils.h"

#include <aiko_includes.h>

namespace nes
{

    #define SET_N(REGISTER) (REGISTER & 0x80)                                               ? N = 1 : N = 0;
    #define SET_Z(REGISTER) (REGISTER == 0x00)                                              ? Z = 1 : Z = 0;
    #define SET_V(REGISTER, RESULT) ((REGISTER ^ memoryFetched) & (A ^ RESULT) & 0x80)      ? V = 1 : V = 0;
    #define SET_C(REGISTER) (REGISTER < 0x100)                                              ? C = 1 : C = 0;

    void Cpu::adc()
    {
        m_currentInstruction = Instruction::adc;
        Word address = A + memoryFetched + C;
        A = address;
        SET_Z(A);
        SET_C(address);
        SET_V(A, address);
    }

    void Cpu:: and()
    {
        m_currentInstruction = Instruction:: and;
        A &= memoryFetched;
        SET_N(A);
        SET_Z(A);
    }

    void Cpu::asl()
    {
        m_currentInstruction = Instruction::asl;
        if (m_currentAddressMode != AddressModes::Implied)
        {
            SET_C(memoryFetched);
            memoryFetched <<= 1;
            SET_N(memoryFetched);
            SET_Z(memoryFetched);
        }
        else
        {
            SET_C(A);
            A <<= 1;
            SET_N(A);
            SET_Z(A);
        }
        program_counter++;
    }

    void Cpu::bcc()
    {
        m_currentInstruction = Instruction::bcc;
        aiko::Log::error("Not Implemented: ", to_string(m_currentInstruction));
    }

    void Cpu::bcs()
    {
        m_currentInstruction = Instruction::bcs;
        aiko::Log::error("Not Implemented: ", to_string(m_currentInstruction));
    }

    void Cpu::beq()
    {
        m_currentInstruction = Instruction::beq;
        aiko::Log::error("Not Implemented: ", to_string(m_currentInstruction));
    }

    void Cpu::bit()
    {
        m_currentInstruction = Instruction::bit;
        aiko::Log::error("Not Implemented: ", to_string(m_currentInstruction));
    }

    void Cpu::bmi()
    {
        m_currentInstruction = Instruction::bmi;
        if (N == 1)
        {
            waitForCycles++;
            Word targetAddress = program_counter + memoryFetched;
            if ((targetAddress & 0xFF00) != (program_counter & 0xFF00))
            {
                waitForCycles++;
            }
            program_counter = targetAddress;
        }
    }

    void Cpu::bne()
    {
        m_currentInstruction = Instruction::bne;
        if (Z == 0)
        {
            waitForCycles++;
            Word targetAddress = program_counter + memoryFetched;
            if ((targetAddress & 0xFF00) != (program_counter & 0xFF00))
            {
                waitForCycles++;
            }
            program_counter = targetAddress;
        }
    }

    void Cpu::bpl()
    {
        m_currentInstruction = Instruction::bpl;
        if (N == 0)
        {
            waitForCycles++;
            Word targetAddress = program_counter + memoryFetched;
            if ((targetAddress & 0xFF00) != (program_counter & 0xFF00))
            {
                waitForCycles++;
            }
            program_counter = targetAddress;
        }
    }

    void Cpu::brk()
    {
        m_currentInstruction = Instruction::brk;
        // 1. Push the address of the next instruction onto the stack.
        program_counter++;
        pushStack(getHigh(program_counter));
        pushStack(getLow(program_counter));
        // 2. Set the Break (B) flag in the status register.
        B = 1;
        // 3. Push the status register onto the stack.
        pushStack(getP());
        // 4. Set the Interrupt Disable (I) flag in the status register.
        I = 1;
        // 5. Load the interrupt vector from addresses $FFFE and $FFFF.
        Word interruptVectorAddress = (getMemory()->read(Word(0xFFFF)) << 8) | getMemory()->read(Word(0xFFFE));
        // 6. Jump to the interrupt service routine specified by the interrupt vector.
        program_counter = interruptVectorAddress;
    }

    void Cpu::bvc()
    {
        m_currentInstruction = Instruction::bvc;
        aiko::Log::error("Not Implemented: ", to_string(m_currentInstruction));
    }

    void Cpu::bvs()
    {
        m_currentInstruction = Instruction::bvs;
        aiko::Log::error("Not Implemented: ", to_string(m_currentInstruction));
    }

    void Cpu::clc()
    {
        m_currentInstruction = Instruction::clc;
        C = 0;
    }

    void Cpu::cld()
    {
        m_currentInstruction = Instruction::cld;
        D = 0;
    }

    void Cpu::cli()
    {
        m_currentInstruction = Instruction::cli;
        I = 0;
    }

    void Cpu::clv()
    {
        m_currentInstruction = Instruction::clv;
        V = 0;
    }

    void Cpu::cmp()
    {
        m_currentInstruction = Instruction::cmp;
        Word result = A - memoryFetched;
        SET_C(result);
        SET_Z(result);
        SET_N(result);
    }

    void Cpu::cpx()
    {
        m_currentInstruction = Instruction::cpx;
        X -= memoryFetched;
        SET_Z(X);
        SET_C(X);
        SET_N(X);
    }

    void Cpu::cpy()
    {
        m_currentInstruction = Instruction::cpy;
        Y -= memoryFetched;
        SET_Z(Y);
        SET_C(Y);
        SET_N(Y);
    }

    void Cpu::dec()
    {
        m_currentInstruction = Instruction::dec;
        memoryFetched--;
        SET_Z(memoryFetched);
        SET_N(memoryFetched);
    }

    void Cpu::dex()
    {
        m_currentInstruction = Instruction::dex;
        X--;
        SET_Z(memoryFetched);
        SET_N(memoryFetched);
    }

    void Cpu::dey()
    {
        m_currentInstruction = Instruction::dey;
        Y--;
        SET_Z(memoryFetched);
        SET_N(memoryFetched);
    }

    void Cpu::eor()
    {
        m_currentInstruction = Instruction::eor;
        A ^= memoryFetched;
        SET_N(A);
        SET_Z(A);
    }

    void Cpu::inc()
    {
        m_currentInstruction = Instruction::inc;
        memoryFetched++;
        SET_Z(memoryFetched);
        SET_N(memoryFetched);
    }

    void Cpu::inx()
    {
        m_currentInstruction = Instruction::inx;
        memoryFetched++;
        SET_Z(memoryFetched);
        SET_N(memoryFetched);
    }

    void Cpu::iny()
    {
        m_currentInstruction = Instruction::iny;
        memoryFetched++;
        SET_Z(memoryFetched);
        SET_N(memoryFetched);
    }

    void Cpu::jmp()
    {
        m_currentInstruction = Instruction::jmp;
        program_counter = memoryFetched;
    }

    void Cpu::jsr()
    {
        m_currentInstruction = Instruction::jsr;
        aiko::Log::error("Not Implemented: ", to_string(m_currentInstruction));
    }

    void Cpu::lda()
    {
        m_currentInstruction = Instruction::lda;
        A = memoryFetched;
        SET_N(A);
        SET_Z(A);
    }

    void Cpu::ldx()
    {
        m_currentInstruction = Instruction::ldx;
        X = memoryFetched;
        SET_N(X);
        SET_Z(X);
    }

    void Cpu::ldy()
    {
        m_currentInstruction = Instruction::ldy;
        Y = memoryFetched;
        SET_N(X);
        SET_Z(X);
    }

    void Cpu::lsr()
    {
        m_currentInstruction = Instruction::lsr;
        program_counter = memoryFetched;
    }

    void Cpu::nop()
    {
        m_currentInstruction = Instruction::nop;
        program_counter++;
    }

    void Cpu::ora()
    {
        m_currentInstruction = Instruction::ora;
        A |= memoryFetched;
        SET_N(A);
        SET_Z(A);
    }

    void Cpu::pha()
    {
        m_currentInstruction = Instruction::pha;
        stack_pointer--;
        Memory* mem = getMemory();
        mem->write(stack_pointer, A);
    }

    void Cpu::php()
    {
        m_currentInstruction = Instruction::php;
        stack_pointer--;
        Memory* mem = getMemory();
        Byte address = 0x0100 + stack_pointer;
        Byte value = getP();
        mem->write(address, value);
    }

    void Cpu::pla()
    {
        m_currentInstruction = Instruction::pla;
        A = popStack();
        SET_N(A);
        SET_Z(A);
    }

    void Cpu::plx()
    {
        m_currentInstruction = Instruction::plx;
        X = popStack();
        SET_N(A);
        SET_Z(A);
    }

    void Cpu::ply()
    {
        m_currentInstruction = Instruction::ply;
        stack_pointer++;
        Memory* mem = getMemory();
        Byte address = 0x0100 + stack_pointer;
        Y = mem->read(address);
        SET_N(A);
        SET_Z(A);
    }

    void Cpu::plp()
    {
        m_currentInstruction = Instruction::plp;
        stack_pointer++;

        // Read processor status value from stack
        Memory* mem = getMemory();
        Byte address = 0x0100 + stack_pointer;
        Byte status = mem->read(address);

        // Update processor status flags
        C = (status >> 0) & 0x01; // Carry flag
        Z = (status >> 1) & 0x01; // Zero flag
        I = (status >> 2) & 0x01; // Interrupt disable flag
        D = (status >> 3) & 0x01; // Decimal mode flag
        B = (status >> 4) & 0x01; // Break command flag
        // Bit 5 is unused and is typically set to 1
        V = (status >> 6) & 0x01; // Overflow flag
        N = (status >> 7) & 0x01; // Negative flag
    }

    void Cpu::rol()
    {
        m_currentInstruction = Instruction::rol;
        aiko::Log::error("Not Implemented: ", to_string(m_currentInstruction));
    }

    void Cpu::ror()
    {
        m_currentInstruction = Instruction::ror;
        aiko::Log::error("Not Implemented: ", to_string(m_currentInstruction));
    }

    void Cpu::rti()
    {
        m_currentInstruction = Instruction::rti;
        aiko::Log::error("Not Implemented: ", to_string(m_currentInstruction));
    }

    void Cpu::rts()
    {
        m_currentInstruction = Instruction::rts;
        aiko::Log::error("Not Implemented: ", to_string(m_currentInstruction));
    }

    void Cpu::sbc()
    {
        m_currentInstruction = Instruction::sbc;
        Word address = static_cast<Word>(A) - memoryFetched - (1 - C);
        A = address;
        SET_N(A);
        SET_V(A, address);
        SET_C(address);
        SET_Z(A);
        program_counter++;
    }

    void Cpu::sec()
    {
        m_currentInstruction = Instruction::sec;
        C = 1;
    }

    void Cpu::sed()
    {
        m_currentInstruction = Instruction::sed;
        D = 1;
    }

    void Cpu::sei()
    {
        m_currentInstruction = Instruction::sei;
        I = 1;
    }

    void Cpu::sta()
    {
        m_currentInstruction = Instruction::sta;
        Memory* mem = getMemory();
        mem->write(memoryFetched, A);
    }

    void Cpu::stx()
    {
        m_currentInstruction = Instruction::stx;
        Memory* mem = getMemory();
        mem->write(memoryFetched, X);
    }

    void Cpu::sty()
    {
        m_currentInstruction = Instruction::sty;
        Memory* mem = getMemory();
        mem->write(memoryFetched, Y);
    }

    void Cpu::tax()
    {
        m_currentInstruction = Instruction::tax;
        X = A;
        SET_N(X);
        SET_Z(X);
    }

    void Cpu::tay()
    {
        m_currentInstruction = Instruction::tay;
        Y = A;
        SET_N(Y);
        SET_Z(Y);
    }
    
    void Cpu::txa()
    {
        m_currentInstruction = Instruction::txa;
        A = X;
        SET_N(A);
        SET_Z(A);
    }

    void Cpu::tya()
    {
        m_currentInstruction = Instruction::tya;
        A = Y;
        SET_N(A);
        SET_Z(A);
    }

    void Cpu::tsx()
    {
        m_currentInstruction = Instruction::tsx;
        X = stack_pointer;
    }

    void Cpu::txs()
    {
        m_currentInstruction = Instruction::txs;
        stack_pointer = X;
    }


}
