#pragma once

#include <stdio.h>
#include <assert.h>
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
    private:
    };

}
