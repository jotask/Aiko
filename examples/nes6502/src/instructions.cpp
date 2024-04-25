#pragma once

#include "nes_types.h"
#include "microprocessor.h"

namespace nes
{

    class Cpu : public Microprocessor
    {
    public:

        virtual void reset() override;
        void clock();
    private:

        Word program_counter;
        Word stack_pointer;

        // Registers
        Byte A, X, Y;

        // StatusFlags
        Byte C : 1;
        Byte Z : 1;
        Byte I : 1;
        Byte D : 1;
        Byte B : 1;
        Byte V : 1;
        Byte N : 1;

    };

}
