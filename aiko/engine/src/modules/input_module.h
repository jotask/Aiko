#pragma once

#include "base_module.h"

namespace aiko
{
    class InputModule : public BaseModule
    {
    public:

        InputModule(Aiko* aiko) : BaseModule(aiko) { };
        virtual ~InputModule() override = default;

    };

}
