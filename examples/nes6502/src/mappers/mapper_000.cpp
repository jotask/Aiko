#include "mappers/mapper_000.h"

namespace nes
{
    Mapper_000::Mapper_000(uint8_t prg_banks, uint8_t chr_banks)
        : Mapper(prg_banks, chr_banks)
    {

    }
    bool Mapper_000::cpu_read(Word address, u32& mapped_addr)
    {
        // if PRGROM is 16KB
        //     CPU Address Bus          PRG ROM
        //     0x8000 -> 0xBFFF: Map    0x0000 -> 0x3FFF
        //     0xC000 -> 0xFFFF: Mirror 0x0000 -> 0x3FFF
        // if PRGROM is 32KB
        //     CPU Address Bus          PRG ROM
        //     0x8000 -> 0xFFFF: Map    0x0000 -> 0x7FFF	
        if (address >= 0x8000 && address <= 0xFFFF)
        {
            mapped_addr = address & (n_prg_banks > 1 ? 0x7FFF : 0x3FFF);
            return true;
        }
        return false;
    }

    bool Mapper_000::cpu_write(Word address, u32& mapped_addr)
    {
        if (address >= 0x8000 && address <= 0xFFFF)
        {
            mapped_addr = address & (n_prg_banks > 1 ? 0x7FFF : 0x3FFF);
            return true;
        }
        return false;
    }

    bool Mapper_000::ppu_read(Word address, u32& mapped_addr)
    {
        // There is no mapping required for PPU
        // PPU Address Bus          CHR ROM
        // 0x0000 -> 0x1FFF: Map    0x0000 -> 0x1FFF
        if (address >= 0x0000 && address <= 0x1FFF)
        {
            mapped_addr = address;
            return true;
        }
        return false;
    }

    bool Mapper_000::ppu_write(Word address, u32& mapped_addr)
    {
        if (address >= 0x0000 && address <= 0x1FFF)
        {
            if (n_chr_banks == 0)
            {
                // Treat as RAM
                mapped_addr = address;
                return true;
            }
        }
        return false;
    }
}
