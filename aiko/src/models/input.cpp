#include "input.h"

#include "core/libs.h"

namespace aiko
{
    
    void Input::update()
    {
    
    }
    
    void Input::reset()
    {

    }

    bool Input::isKeyPressed(Key key)
    {
        return IsKeyPressed((KeyboardKey)key);
    }

}
