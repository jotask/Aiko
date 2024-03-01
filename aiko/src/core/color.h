#pragma once

#include "shared/math.h"

namespace aiko
{

    class Color
    {
    public:
        Color() : Color(0.0f, 1.0f) { };
        Color(float rgb, float a) : Color(rgb, rgb, rgb, a) { };
        Color(float rgba) : Color(rgba, rgba, rgba, rgba) { };
        Color(float r, float g, float b, float a) : r(r), g(b), b(b), a(a) { };
        float r;
        float g;
        float b;
        float a;
    };

}

