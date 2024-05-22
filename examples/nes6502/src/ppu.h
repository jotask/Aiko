#pragma once

#include "microprocessor.h"
#include "nes_types.h"

namespace nes
{

    class Ppu : public Microprocessor
    {
    public:
        virtual void reset() override;

    public:
        // Communications with Main Bus
        Byte cpu_read(Word addr, bool rdonly = false);
        void cpu_write(Word addr, Byte  data);

        // Communications with PPU Bus
        Byte ppu_read(Word addr, bool rdonly = false);
        void ppu_write(Word addr, Byte data);
    };

}
