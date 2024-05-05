#include "cpu.h"

#include "bus.h"
#include "memory.h"

namespace nes
{

    #define SET_N(REGISTER) (REGISTER & 0x80)               ? N = 1 : N = 0;
    #define SET_Z(REGISTER) (REGISTER == 0)                 ? Z = 1 : Z = 0;
    #define SET_C(REGISTER) (REGISTER >= memoryFetched )    ? C = 1 : C = 0;

    void Cpu::adc()
    {
        m_currentInstruction = Instruction::adc;
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
    }

    void Cpu::bcc()
    {
        m_currentInstruction = Instruction::bcc;
    }

    void Cpu::bcs()
    {
        m_currentInstruction = Instruction::bcs;
    }

    void Cpu::beq()
    {
        m_currentInstruction = Instruction::beq;
    }

    void Cpu::bit()
    {
        m_currentInstruction = Instruction::bit;
    }

    void Cpu::bmi()
    {
        m_currentInstruction = Instruction::bmi;
    }

    void Cpu::bne()
    {
        m_currentInstruction = Instruction::bne;
    }

    void Cpu::bpl()
    {
        m_currentInstruction = Instruction::bpl;
    }

    void Cpu::brk()
    {
        m_currentInstruction = Instruction::brk;
    }

    void Cpu::bvc()
    {
        m_currentInstruction = Instruction::bvc;
    }

    void Cpu::bvs()
    {
        m_currentInstruction = Instruction::bvs;
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
    }

    void Cpu::jsr()
    {
        m_currentInstruction = Instruction::jsr;
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
        stack_pointer++;
        Memory* mem = getMemory();
        Byte address = 0x0100 + stack_pointer;
        A = mem->read(address);
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
    }

    void Cpu::ror()
    {
        m_currentInstruction = Instruction::ror;
    }

    void Cpu::rti()
    {
        m_currentInstruction = Instruction::rti;
    }

    void Cpu::rts()
    {
        m_currentInstruction = Instruction::rts;
    }

    void Cpu::sbc()
    {
        m_currentInstruction = Instruction::sbc;
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
