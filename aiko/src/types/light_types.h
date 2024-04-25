#pragma once

#include <vector>
#include "shared/math.h"

namespace aiko
{

    enum LightType
    {
        Dir,
        Point,
        Spot,
    };

    struct LightData
    {

        LightType type;

        vec3 position;
        vec3 direction;
        vec3 color;
        float intensity;

        float cutOff;
        float outerCutOff;

        vec3 ambient;
        vec3 diffuse;
        vec3 specular;

        float constant;
        float linear;
        float quadratic;

    };

}

