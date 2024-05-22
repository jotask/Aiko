#pragma once

#include "nes/nes_types.h"

namespace nes
{

    class Mapper
    {
    public:

        Mapper(Byte prg_bank, Byte chr_bank);
        virtual ~Mapper() = default;

        virtual bool cpu_read(Word address, u32& mapped_addr) = 0;
        virtual bool cpu_write(Word address, u32& mapped_addr) = 0;

        virtual bool ppu_read(Word address, u32& mapped_addr) = 0 ;
        virtual bool ppu_write(Word address, u32& mapped_addr) = 0;

    protected:

        Byte n_prg_banks;
        Byte n_chr_banks;


    };

}
