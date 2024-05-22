#pragma once

#include "nes/cartridge/mappers/mapper.h"

namespace nes
{
    class Mapper_000 : public Mapper
    {
    public:
        Mapper_000(Byte prg_banks, Byte chr_banks);
        virtual ~Mapper_000() = default;

        virtual bool cpu_read(Word address, u32& mapped_addr) override;
        virtual bool cpu_write(Word address, u32& mapped_addr) override;
        virtual bool ppu_read(Word address, u32& mapped_addr) override;
        virtual bool ppu_write(Word address, u32& mapped_addr) override;

    };
}
