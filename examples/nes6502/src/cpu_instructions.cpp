#include "cpu.h"

#include "bus.h"
#include "memory.h"

namespace nes
{

#define SET_N(REGISTER) (REGISTER & 0x80) ? N = 1 : N = 0;
#define SET_Z(REGISTER) (REGISTER == 0)   ? Z = 1 : Z = 0;


    void Cpu::adc()
    {
        m_currentInstruction = Instruction::adc;
    }

    void Cpu:: and()
    {
        m_currentInstruction = Instruction:: and;
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
    }

    void Cpu::cld()
    {
        m_currentInstruction = Instruction::cld;
    }

    void Cpu::cli()
    {
        m_currentInstruction = Instruction::cli;
    }

    void Cpu::clv()
    {
        m_currentInstruction = Instruction::clv;
    }

    void Cpu::cmp()
    {
        m_currentInstruction = Instruction::cmp;
    }

    void Cpu::cpx()
    {
        m_currentInstruction = Instruction::cpx;
    }

    void Cpu::cpy()
    {
        m_currentInstruction = Instruction::cpy;
    }

    void Cpu::dec()
    {
        m_currentInstruction = Instruction::dec;
    }

    void Cpu::dex()
    {
        m_currentInstruction = Instruction::dex;
    }

    void Cpu::dey()
    {
        m_currentInstruction = Instruction::dey;
    }

    void Cpu::eor()
    {
        m_currentInstruction = Instruction::eor;
    }

    void Cpu::inc()
    {
        m_currentInstruction = Instruction::inc;
    }

    void Cpu::inx()
    {
        m_currentInstruction = Instruction::inx;
    }

    void Cpu::iny()
    {
        m_currentInstruction = Instruction::iny;
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
    }

    void Cpu::nop()
    {
        m_currentInstruction = Instruction::nop;
    }

    void Cpu::ora()
    {
        m_currentInstruction = Instruction::ora;
    }

    void Cpu::pha()
    {
        m_currentInstruction = Instruction::pha;
    }

    void Cpu::php()
    {
        m_currentInstruction = Instruction::php;
    }

    void Cpu::pla()
    {
        m_currentInstruction = Instruction::pla;
    }

    void Cpu::plp()
    {
        m_currentInstruction = Instruction::plp;
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
    }

    void Cpu::sed()
    {
        m_currentInstruction = Instruction::sed;
    }

    void Cpu::sei()
    {
        m_currentInstruction = Instruction::sei;
    }

    void Cpu::sta()
    {
        m_currentInstruction = Instruction::sta;
    }

    void Cpu::stx()
    {
        m_currentInstruction = Instruction::stx;
    }

    void Cpu::sty()
    {
        m_currentInstruction = Instruction::sty;
    }

    void Cpu::tax()
    {
        m_currentInstruction = Instruction::tax;
    }

    void Cpu::tay()
    {
        m_currentInstruction = Instruction::tay;
    }

    void Cpu::tsx()
    {
        m_currentInstruction = Instruction::tsx;
    }

    void Cpu::txa()
    {
        m_currentInstruction = Instruction::txa;
    }

    void Cpu::txs()
    {
        m_currentInstruction = Instruction::txs;
    }

    void Cpu::tya()
    {
        m_currentInstruction = Instruction::tya;
    }

}
