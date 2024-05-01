#pragma once

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "nes_types.h"
#include "microprocessor.h"

namespace nes
{

    class Memory : public Microprocessor
    {
    public:
        virtual void reset() override;
        static constexpr u32 MAX_MEM = 1024 * 64;
        Byte data[MAX_MEM];

        // read 1 byte
        Byte operator[] (Byte address) const
        {
            assert(address < MAX_MEM);
            return data[address];
        }
    private:
    };

}
