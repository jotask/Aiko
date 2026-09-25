#pragma once

#include <array>

#include <aiko_types.h>
#include <assets/asset_id.h>
#include <math/math.h>

namespace aiko
{

    struct FontGlyph
    {
        vec2 uvMin{0.0f};
        vec2 uvMax{0.0f};

        vec2 offset{0.0f};
        vec2 size{0.0f};

        float advance = 0.0f;
    };

    class Font
    {
    public:

        static constexpr int FirstCharacter = 32;
        static constexpr int LastCharacter = 126;
        static constexpr int CharacterCount = LastCharacter - FirstCharacter + 1;

        bool isValid() const
        {
            return m_atlasTexture != InvalidAssetId;
        }

        const AssetId& atlasTexture() const
        {
            return m_atlasTexture;
        }

        float pixelSize() const
        {
            return m_pixelSize;
        }

        float ascent() const
        {
            return m_ascent;
        }

        float descent() const
        {
            return m_descent;
        }

        float lineHeight() const
        {
            return m_lineHeight;
        }

        const FontGlyph* glyph(char character) const
        {
            const int code = static_cast<unsigned char>(character);

            if (code < FirstCharacter || code > LastCharacter)
            {
                return nullptr;
            }

            return &m_glyphs[static_cast<size_t>(code - FirstCharacter)];
        }

    private:

        friend class AssetContext;

        AssetId m_atlasTexture = InvalidAssetId;

        float m_pixelSize = 0.0f;

        float m_ascent = 0.0f;
        float m_descent = 0.0f;
        float m_lineHeight = 0.0f;

        std::array<FontGlyph, CharacterCount> m_glyphs{};
    };

}
