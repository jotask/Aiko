#include "input.h"

#include "core/libs.h"

namespace aiko
{

    bool Input::isKeyPressed(Key key)
    {
        return pressedKeys[key];
    }

    vec2 Input::getMousePosition()
    {
        return mousePosition;
    }

}
