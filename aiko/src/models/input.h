#pragma once

#include "aiko_types.h"
#include "shared/singleton.h"
#include "types/inputs.h"

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
