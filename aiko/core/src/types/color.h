#pragma once

#include <aiko_types.h>
#include <core/random.h>

#include "math/math_vector.h"

namespace aiko
{

    class Color
    {
    public:
        constexpr Color() : Color(0.0f, 1.0f) { };
        constexpr Color(float rgb, float a) : Color(rgb, rgb, rgb, a) { };
        constexpr Color(float rgba) : Color(rgba, rgba, rgba, rgba) { };
        constexpr Color(float r, float g, float b) : r(r), g(g), b(b), a(1.0f) { };
        constexpr Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) { };
        float r;
        float g;
        float b;
        float a;

        constexpr bool operator==(const Color& other) { return r == other.r && g == other.g && b == other.b && a == other.a; }
        constexpr bool operator!=(const Color& other) { return !(*this == other); }

        u32 rgba() const
        {
            u8 red = static_cast<u8>(r * 255.0f);
            u8 green = static_cast<u8>(g * 255.0f);
            u8 blue = static_cast<u8>(b * 255.0f);
            u8 alpha = static_cast<u8>(a * 255.0f);
            u32 rgba = (red << 24) | (green << 16) | (blue << 8) | alpha;
            return rgba;
        }

        vec4 toVec4() const { return {r, g, b, a}; }

        static Color getRandomColor()
        {
            return {
                utils::getRandomValue(0.0f, 1.0f),
                utils::getRandomValue(0.0f, 1.0f),
                utils::getRandomValue(0.0f, 1.0f),
                utils::getRandomValue(0.0f, 1.0f),
            };
        }

    };

    // Thanks raysan
    // Some Basic Colors
    // NOTE: Custom raylib color palette for amazing visuals on WHITE background
    constexpr Color LIGHTGRAY  { 200 / 255.0f    , 200 / 255.0f   , 200 / 255.0f     , 255 / 255.0f };   // Light Gray
    constexpr Color GRAY       { 130 / 255.0f    , 130 / 255.0f   , 130 / 255.0f     , 255 / 255.0f };   // Gray
    constexpr Color DARKGRAY   { 80 / 255.0f     , 80 / 255.0f    , 80 / 255.0f      , 255 / 255.0f };   // Dark Gray
    constexpr Color YELLOW     { 253 / 255.0f    , 249 / 255.0f   , 0 / 255.0f       , 255 / 255.0f };   // Yellow
    constexpr Color GOLD       { 255 / 255.0f    , 203 / 255.0f   , 0 / 255.0f       , 255 / 255.0f };   // Gold
    constexpr Color ORANGE     { 255 / 255.0f    , 161 / 255.0f   , 0 / 255.0f       , 255 / 255.0f };   // Orange
    constexpr Color PINK       { 255 / 255.0f    , 109 / 255.0f   , 194 / 255.0f     , 255 / 255.0f };   // Pink
    constexpr Color RED        { 230 / 255.0f    , 41 / 255.0f    , 55 / 255.0f      , 255 / 255.0f };   // Red
    constexpr Color MAROON     { 190 / 255.0f    , 33 / 255.0f    , 55 / 255.0f      , 255 / 255.0f };   // Maroon
    constexpr Color GREEN      { 0 / 255.0f      , 228 / 255.0f   , 48 / 255.0f      , 255 / 255.0f };   // Green
    constexpr Color LIME       { 0 / 255.0f      , 158 / 255.0f   , 47 / 255.0f      , 255 / 255.0f };   // Lime
    constexpr Color DARKGREEN  { 0 / 255.0f      , 117 / 255.0f   , 44 / 255.0f      , 255 / 255.0f };   // Dark Green
    constexpr Color SKYBLUE    { 102 / 255.0f    , 191 / 255.0f   , 255 / 255.0f     , 255 / 255.0f };   // Sky Blue
    constexpr Color BLUE       { 0 / 255.0f      , 121 / 255.0f   , 241 / 255.0f     , 255 / 255.0f };   // Blue
    constexpr Color DARKBLUE   { 0 / 255.0f      , 82 / 255.0f    , 172 / 255.0f     , 255 / 255.0f };   // Dark Blue
    constexpr Color PURPLE     { 200.0f / 255.0f , 122.0f / 255.0f, 255.0f / 255.0f  , 255 / 255.0f };   // Purple
    constexpr Color VIOLET     { 135 / 255.0f    , 60 / 255.0f    , 190 / 255.0f     , 255 / 255.0f };   // Violet
    constexpr Color DARKPURPLE { 112 / 255.0f    , 31 / 255.0f    , 126 / 255.0f     , 255 / 255.0f };   // Dark Purple
    constexpr Color BEIGE      { 211 / 255.0f    , 176 / 255.0f   , 131 / 255.0f     , 255 / 255.0f };   // Beige
    constexpr Color BROWN      { 127 / 255.0f    , 106 / 255.0f   , 79 / 255.0f      , 255 / 255.0f };   // Brown
    constexpr Color DARKBROWN  { 76 / 255.0f     , 63 / 255.0f    , 47 / 255.0f      , 255 / 255.0f };   // Dark Brown
    constexpr Color WHITE      { 255 / 255.0f    , 255 / 255.0f   , 255 / 255.0f     , 255 / 255.0f };   // White
    constexpr Color BLACK      { 0 / 255.0f      , 0 / 255.0f     , 0 / 255.0f       , 255 / 255.0f };   // Black
    constexpr Color BLANK      { 0 / 255.0f      , 0 / 255.0f     , 0 / 255.0f       , 0 / 255.0f   };   // Blank (Transparent)
    constexpr Color MAGENTA    { 255 / 255.0f    , 0 / 255.0f     , 255 / 255.0f     , 255 / 255.0f };   // Magenta
    constexpr Color CYAN       { 0 / 255.0f      , 255 / 255.0f   , 255 / 255.0f     , 255 / 255.0f };   // Cyan
    constexpr Color RAYWHITE   { 245 / 255.0f    , 245 / 255.0f   , 245 / 255.0f     , 255 / 255.0f };   // My own White (raylib logo)

    #define AIKO_DEFAULT_CLEAR_COLOR RAYWHITE
    #define AIKO_DEFAULT_PRIMITIVE_COLOR WHITE
    #define AIKO_DEFAULT_MESH_COLOR CYAN

}

