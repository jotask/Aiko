#pragma once

#include "mappers/mapper.h"

namespace nes
{
    class Mapper_000 : public Mapper
    {
    public:
        Mapper_000(uint8_t prg_banks, uint8_t chr_banks);
        virtual ~Mapper_000() = default;

        virtual bool cpu_read(Word address, u32& mapped_addr) override;
        virtual bool cpu_write(Word address, u32& mapped_addr) override;
        virtual bool ppu_read(Word address, u32& mapped_addr) override;
        virtual bool ppu_write(Word address, u32& mapped_addr) override;

    };
}
