#include "transform_component.h"

#include <GLFW/glfw3.h>

#include "models/camera.h"
#include "shared/math_transform.h"

namespace aiko
{
    
    Transform::Transform()
        : Component("Transform")
        , parent(nullptr)
    {
    
    }

}
