#pragma once

#include "aiko_types.h"
#include "shared/math.h"
#include "shared/singleton.h"
#include "types/inputs.h"

namespace aiko
{
    
    class Input : public Singleton<Input>
    {
        friend class InputModule;
    public:
    
        bool isKeyPressed(Key);
        vec2 getMousePosition();

    private:

        vec2 mousePosition;
    
    };

}
