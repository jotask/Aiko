#include "vector.h"

#include <math.h>

namespace aiko
{

    float Vector2D::distance(vec2 v1, vec2 v2)
    {
        {
            float dx = v2.x - v1.x;;
            float dy = v2.y - v1.y;;
            return sqrt(dx * dx + dy * dy);;
        }
    }

}
