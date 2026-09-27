#pragma once

#include <math/math_bounds.h>

namespace aiko
{
    class GameObject;
}

namespace aiko::editor
{

    class EditorContext;
    bool calculateSceneObjectBounds(EditorContext& context, const GameObject& object, Bounds& bounds);

}
