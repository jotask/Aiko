#pragma once

#include "aiko_types.h"
#include "shared/math.h"

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

        constexpr bool Color::operator==(const Color& other) { return r == other.r && g == other.g && b == other.b && a == other.a; }
        constexpr bool Color::operator!=(const Color& other) { return !(*this == other); }

        const u32 rgba()
        {
            uint8_t red = static_cast<uint8_t>(r * 255.0f);
            uint8_t green = static_cast<uint8_t>(g * 255.0f);
            uint8_t blue = static_cast<uint8_t>(b * 255.0f);
            uint8_t alpha = static_cast<uint8_t>(a * 255.0f);
            uint32_t rgba = (red << 24) | (green << 16) | (blue << 8) | alpha;
            return rgba;
        }

    };

    // Thanks raysan
    // Some Basic Colors
    // NOTE: Custom raylib color palette for amazing visuals on WHITE background
    #define LIGHTGRAY  CLITERAL(Color){ 200 / 255.0f    , 200 / 255.0f   , 200 / 255.0f     , 255 / 255.0f }   // Light Gray
    #define GRAY       CLITERAL(Color){ 130 / 255.0f    , 130 / 255.0f   , 130 / 255.0f     , 255 / 255.0f }   // Gray
    #define DARKGRAY   CLITERAL(Color){ 80 / 255.0f     , 80 / 255.0f    , 80 / 255.0f      , 255 / 255.0f }   // Dark Gray
    #define YELLOW     CLITERAL(Color){ 253 / 255.0f    , 249 / 255.0f   , 0 / 255.0f       , 255 / 255.0f }   // Yellow
    #define GOLD       CLITERAL(Color){ 255 / 255.0f    , 203 / 255.0f   , 0 / 255.0f       , 255 / 255.0f }   // Gold
    #define ORANGE     CLITERAL(Color){ 255 / 255.0f    , 161 / 255.0f   , 0 / 255.0f       , 255 / 255.0f }   // Orange
    #define PINK       CLITERAL(Color){ 255 / 255.0f    , 109 / 255.0f   , 194 / 255.0f     , 255 / 255.0f }   // Pink
    #define RED        CLITERAL(Color){ 230 / 255.0f    , 41 / 255.0f    , 55 / 255.0f      , 255 / 255.0f }   // Red
    #define MAROON     CLITERAL(Color){ 190 / 255.0f    , 33 / 255.0f    , 55 / 255.0f      , 255 / 255.0f }   // Maroon
    #define GREEN      CLITERAL(Color){ 0 / 255.0f      , 228 / 255.0f   , 48 / 255.0f      , 255 / 255.0f }   // Green
    #define LIME       CLITERAL(Color){ 0 / 255.0f      , 158 / 255.0f   , 47 / 255.0f      , 255 / 255.0f }   // Lime
    #define DARKGREEN  CLITERAL(Color){ 0 / 255.0f      , 117 / 255.0f   , 44 / 255.0f      , 255 / 255.0f }   // Dark Green
    #define SKYBLUE    CLITERAL(Color){ 102 / 255.0f    , 191 / 255.0f   , 255 / 255.0f     , 255 / 255.0f }   // Sky Blue
    #define BLUE       CLITERAL(Color){ 0 / 255.0f      , 121 / 255.0f   , 241 / 255.0f     , 255 / 255.0f }   // Blue
    #define DARKBLUE   CLITERAL(Color){ 0 / 255.0f      , 82 / 255.0f    , 172 / 255.0f     , 255 / 255.0f }   // Dark Blue
    #define PURPLE     CLITERAL(Color){ 200 / 255.0f    , 122 / 255.0f   , 255 / 255.0f     , 255 / 255.0f }   // Purple
    #define VIOLET     CLITERAL(Color){ 135 / 255.0f    , 60 / 255.0f    , 190 / 255.0f     , 255 / 255.0f }   // Violet
    #define DARKPURPLE CLITERAL(Color){ 112 / 255.0f    , 31 / 255.0f    , 126 / 255.0f     , 255 / 255.0f }   // Dark Purple
    #define BEIGE      CLITERAL(Color){ 211 / 255.0f    , 176 / 255.0f   , 131 / 255.0f     , 255 / 255.0f }   // Beige
    #define BROWN      CLITERAL(Color){ 127 / 255.0f    , 106 / 255.0f   , 79 / 255.0f      , 255 / 255.0f }   // Brown
    #define DARKBROWN  CLITERAL(Color){ 76 / 255.0f     , 63 / 255.0f    , 47 / 255.0f      , 255 / 255.0f }   // Dark Brown
    #define WHITE      CLITERAL(Color){ 255 / 255.0f    , 255 / 255.0f   , 255 / 255.0f     , 255 / 255.0f }   // White
    #define BLACK      CLITERAL(Color){ 0 / 255.0f      , 0 / 255.0f     , 0 / 255.0f       , 255 / 255.0f }   // Black
    #define BLANK      CLITERAL(Color){ 0 / 255.0f      , 0 / 255.0f     , 0 / 255.0f       , 0 / 255.0f   }   // Blank (Transparent)
    #define MAGENTA    CLITERAL(Color){ 255 / 255.0f    , 0 / 255.0f     , 255 / 255.0f     , 255 / 255.0f }   // Magenta
    #define RAYWHITE   CLITERAL(Color){ 245 / 255.0f    , 245 / 255.0f   , 245 / 255.0f     , 255 / 255.0f }   // My own White (raylib logo)

}

