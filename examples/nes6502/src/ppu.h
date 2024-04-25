#pragma once

#include "microprocessor.h"

namespace nes
{

    class Ppu : public Microprocessor
    {
    public:
        virtual void reset() override;
    };

}
