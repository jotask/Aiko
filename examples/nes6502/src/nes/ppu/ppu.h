#pragma once

#include "nes/microprocessor.h"
#include "nes/nes_types.h"

namespace nes
{

    class Ppu : public Microprocessor
    {
        friend class Nes;
    private:
        virtual void reset() override;
        void clock();
        Byte table_name[2][1024];
        Byte table_pattern[2][4096];
        Byte table_palette[32];

        Word scan_line = 0;
        Word cycle = 0;

    public:

        bool frame_complete = false;

        // Communications with Main Bus
        Byte cpu_read(Word addr, bool readonly = false);
        void cpu_write(Word addr, Byte  data);

        // Communications with PPU Bus
        Byte ppu_read(Word addr, bool readonly = false);
        void ppu_write(Word addr, Byte data);
    };

}
