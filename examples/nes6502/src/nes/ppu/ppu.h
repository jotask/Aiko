#pragma once

#include <array>

#include <aiko_includes.h>

#include "nes/microprocessor.h"
#include "nes/nes_types.h"

namespace nes
{

    class Ppu : public Microprocessor
    {
        friend class Nes;
    public:

        using Pixels = std::vector<aiko::Color>;

        Ppu();
        virtual ~Ppu() = default;

        Pixels m_pixels;

        bool frame_complete = true;

        Pixels getPixels();

        // Communications with Main Bus
        Byte cpu_read(Word addr, bool readonly = false);
        void cpu_write(Word addr, Byte  data);

        // Communications with PPU Bus
        Byte ppu_read(Word addr, bool readonly = false);
        void ppu_write(Word addr, Byte data);

    private:
        virtual void reset() override;
        void clock();
        Byte table_name[2][1024];
        Byte table_pattern[2][4096];
        Byte table_palette[32];

        Word scan_line = 0;
        Word cycle = 0;
        std::array<Byte, 32> palette_table;
        std::array<Byte, 2048> ram;
    };

}
