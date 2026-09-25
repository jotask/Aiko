#pragma once

#include <string_view>
#include <vector>

#include <math/math.h>

namespace aiko
{

    class Font;

    struct TextGlyphQuad
    {
        vec2 position{0.0f};
        vec2 size{0.0f};

        vec2 uvMin{0.0f};
        vec2 uvMax{0.0f};
    };

    struct TextLayoutResult
    {
        std::vector<TextGlyphQuad> glyphs;

        vec2 size{0.0f};
    };

    TextLayoutResult layoutText(const Font& font, std::string_view text, float fontSize);

}
