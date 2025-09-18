#pragma once

#include "shared/singleton.h"

namespace aiko
{
    class Input : public Singleton<Input>
    {
        friend class InputSystem;
        friend class Aiko;
    public:

    private:
    };

}
