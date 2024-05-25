#pragma once

#include <stdio.h>
#include <assert.h>
#include <vector>
#include <stdlib.h>

#include <aiko_includes.h>

#include "nes/nes_types.h"
#include "nes/microprocessor.h"
#include "mappers/mapper.h"

namespace nes
{

    class Cartridge : public Microprocessor
    {
    public:
        void load(const char*);
        virtual void reset() override;

        bool cpu_read(uint16_t addr, uint8_t& data);
        bool cpu_write(uint16_t addr, uint8_t data);

        bool ppu_read(uint16_t addr, uint8_t& data);
        bool ppu_write(uint16_t addr, uint8_t data);

        std::vector<Byte> getCHR() const;

    private:
        enum class Mirroring
        {
            Vertical,
            Horizontal,
            Four_Screen,
        };
        std::vector<Byte> PRG_Memory;
        std::vector<Byte> CHR_Memory;
        aiko::AikoPtr<Mapper> mapper;
    };

}
