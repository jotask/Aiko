#pragma once

#include "nes/microprocessor.h"
#include "nes/nes_types.h"

namespace nes
{

    class Ppu : public Microprocessor
    {
    public:
        virtual void reset() override;

    public:
        // Communications with Main Bus
        Byte cpu_read(Word addr, bool readonly = false);
        void cpu_write(Word addr, Byte  data);

        // Communications with PPU Bus
        Byte ppu_read(Word addr, bool readonly = false);
        void ppu_write(Word addr, Byte data);
    };

}
