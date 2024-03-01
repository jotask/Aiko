#pragma once

#include "aiko_types.h"
#include "shared/singleton.h"
#include "core/inputs.h"

namespace aiko
{
    
    class Input : public Singleton<Input>
    {
        friend class Aiko;
    public:
    
        bool isKeyPressed(Key);

    private:

        void update();
    
        void reset();
    
    };

}
