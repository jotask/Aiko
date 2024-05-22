#pragma once

#include <stdio.h>
#include <assert.h>
#include <vector>
#include <stdlib.h>

#include "nes_types.h"
#include "microprocessor.h"

namespace nes
{

    class Cartridge : public Microprocessor
    {
    public:
        void load(const char*);
        void load(std::vector<Byte>&);
        virtual void reset() override;

        bool cpu_read(uint16_t addr, uint8_t& data);
        bool cpu_write(uint16_t addr, uint8_t data);

        bool ppu_read(uint16_t addr, uint8_t& data);
        bool ppu_write(uint16_t addr, uint8_t data);

    private:
    };

}
