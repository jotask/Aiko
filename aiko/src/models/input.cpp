#include "input.h"

#include "core/libs.h"

namespace aiko
{

    bool Input::isKeyPressed(Key key)
    {
        return IsKeyPressed((KeyboardKey)key);
    }

    vec2 Input::getMousePosition()
    {
        return mousePosition;
    }

}
