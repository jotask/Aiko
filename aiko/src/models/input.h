#pragma once

#include "aiko_types.h"
#include "core/singleton.h"

#include <raylib.h>

namespace aiko
{
    
    class Input : public Singleton<Input>
    {
        friend class Aiko;
    public:

        using Key = KeyboardKey;
    
        bool isKeyPressed(Key);

    private:

        void update();
    
        void reset();
    
    };

}
