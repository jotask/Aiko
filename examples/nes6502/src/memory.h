#pragma once

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "nes_types.h"
#include "microprocessor.h"

namespace nes
{

    class Memory : public Microprocessor
    {
        friend class Cartridge;
    public:
        static constexpr u32 MAX_MEM = 1024 * 64; // 64KB

        virtual void reset() override;

        Byte read(Byte);
        void write(Byte, Byte);

        Byte read(Word);
        void write(Word, Word);

    private:
        Byte data[MAX_MEM];
    };

}
