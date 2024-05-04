#include "cpu.h"

#include "bus.h"
#include "memory.h"

namespace nes
{

#define SET_N(REGISTER) (REGISTER & 0x80) ? N = 1 : N = 0;
#define SET_Z(REGISTER) (REGISTER == 0)   ? Z = 1 : Z = 0;


    void Cpu::adc()
    {
    }

    void Cpu:: and()
    {
    }

    void Cpu::asl()
    {
    }

    void Cpu::bcc()
    {
    }

    void Cpu::bcs()
    {
    }

    void Cpu::beq()
    {
    }

    void Cpu::bit()
    {
    }

    void Cpu::bmi()
    {
    }

    void Cpu::bne()
    {
    }

    void Cpu::bpl()
    {
    }

    void Cpu::brk()
    {
    }

    void Cpu::bvc()
    {
    }

    void Cpu::bvs()
    {
    }

    void Cpu::clc()
    {
    }

    void Cpu::cld()
    {
    }

    void Cpu::cli()
    {
    }

    void Cpu::clv()
    {
    }

    void Cpu::cmp()
    {
    }

    void Cpu::cpx()
    {
    }

    void Cpu::cpy()
    {
    }

    void Cpu::dec()
    {
    }

    void Cpu::dex()
    {
    }

    void Cpu::dey()
    {
    }

    void Cpu::eor()
    {
    }

    void Cpu::inc()
    {
    }

    void Cpu::inx()
    {
    }

    void Cpu::iny()
    {
    }

    void Cpu::jmp()
    {
    }

    void Cpu::jsr()
    {
    }

    void Cpu::lda()
    {
        A = memoryFetched;
        SET_N(A);
        SET_Z(A);
    }

    void Cpu::ldx()
    {
        X = memoryFetched;
        SET_N(X);
        SET_Z(X);
    }

    void Cpu::ldy()
    {
        Y = memoryFetched;
        SET_N(X);
        SET_Z(X);
    }

    void Cpu::lsr()
    {
    }

    void Cpu::nop()
    {
    }

    void Cpu::ora()
    {
    }

    void Cpu::pha()
    {
    }

    void Cpu::php()
    {
    }

    void Cpu::pla()
    {
    }

    void Cpu::plp()
    {
    }

    void Cpu::rol()
    {
    }

    void Cpu::ror()
    {
    }

    void Cpu::rti()
    {
    }

    void Cpu::rts()
    {
    }

    void Cpu::sbc()
    {
    }

    void Cpu::sec()
    {
    }

    void Cpu::sed()
    {
    }

    void Cpu::sei()
    {
    }

    void Cpu::sta()
    {
    }

    void Cpu::stx()
    {
    }

    void Cpu::sty()
    {
    }

    void Cpu::tax()
    {
    }

    void Cpu::tay()
    {
    }

    void Cpu::tsx()
    {
    }

    void Cpu::txa()
    {
    }

    void Cpu::txs()
    {
    }

    void Cpu::tya()
    {
    }

}
