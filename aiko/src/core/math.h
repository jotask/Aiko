#pragma once

namespace aiko
{

    class ivec2
    {
    public:
        ivec2() : ivec2(0) { };
        ivec2(int xyz) : ivec2(xyz, xyz) { };
        ivec2(int x, int y) : x(x), y(y) { };
        int x;
        int y;
    };

    class vec2
    {
    public:
        vec2() : vec2(0.0f) { };
        vec2(float xyz) : vec2(xyz, xyz) { };
        vec2(float x, float y) : x(x), y(y) { };
        float x;
        float y;
    };

    class vec3
    {
    public:
        vec3() : vec3(0.0f) { };
        vec3(float xyz) : vec3(xyz, xyz, xyz) { };
        vec3(float x, float y, float z) : x(x), y(y), z(z) { };
        float x;
        float y;
        float z;
    };

    class vec4
    {
    public:
        vec4() : vec4(0.0f) { };
        vec4(float xyzw) : vec4(xyzw, xyzw, xyzw, xyzw) { };
        vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) { };
        float x;
        float y;
        float z;
        float w;
    };

}

