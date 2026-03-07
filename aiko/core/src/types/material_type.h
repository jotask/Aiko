#pragma once

#include <vector>
#include "math/math.h"

namespace aiko
{

    struct MaterialData
    {
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float shininess;
    };

}

