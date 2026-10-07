#include "text_layout.h"

#include <algorithm>

#include "models/font.h"

namespace aiko
{

    TextLayoutResult layoutText(const Font& font, std::string_view text, float fontSize)
    {
        TextLayoutResult result;

        if (font.isValid() == false || font.pixelSize() <= 0.0f || fontSize <= 0.0f)
        {
            return result;
        }

        const float scale = fontSize / font.pixelSize();

        const float lineHeight = font.lineHeight() * scale;

        float penX = 0.0f;
        float baseline = font.ascent() * scale;

        float maxWidth = 0.0f;

        for (char character : text)
        {
            if (character == '\n')
            {
                maxWidth = std::max(maxWidth, penX);
                penX = 0.0f;
                baseline += lineHeight;

                continue;
            }

            const FontGlyph* glyph = font.glyph(character);

            if (glyph == nullptr)
            {
                continue;
            }

            if (glyph->size.x > 0.0f && glyph->size.y > 0.0f)
            {
                result.glyphs.push_back(
                {
                    .position =
                    {
                        penX + glyph->offset.x * scale,
                        baseline + glyph->offset.y * scale
                    },

                    .size =
                    {
                        glyph->size.x * scale,
                        glyph->size.y * scale
                    },

                    .uvMin = glyph->uvMin,
                    .uvMax = glyph->uvMax
                });
            }

            penX += glyph->advance * scale;
        }

        maxWidth = std::max(maxWidth, penX);

        result.size =
        {
            maxWidth,
            baseline - font.ascent() * scale + lineHeight
        };

        return result;
    }

}
