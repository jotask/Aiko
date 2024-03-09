#pragma once

#include "base_module.h"
#include "types/inputs.h"
#include "shared/math.h"

namespace aiko
{
    
    class InputModule : public BaseModule
    {
    public:
    
        InputModule() = default;
        virtual ~InputModule() = default;

        bool isKeyPressed(Key) const;
        vec2 getMousePosition() const;
        bool isMouseButtonPressed(MouseButton button) const;

    protected:

        virtual void init() override;
        virtual void preUpdate() override;
    };

}
