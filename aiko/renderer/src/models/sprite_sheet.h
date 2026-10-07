#pragma once

#include <aiko_types.h>

#include "models/texture_region.h"

namespace aiko
{

    class SpriteSheet
    {
    public:

        SpriteSheet(
            uint32_t textureWidth,
            uint32_t textureHeight,
            uint32_t frameWidth,
            uint32_t frameHeight)
            : m_textureWidth(textureWidth)
            , m_textureHeight(textureHeight)
            , m_frameWidth(frameWidth)
            , m_frameHeight(frameHeight)
        {
            AIKO_ASSERT(m_textureWidth > 0, "Sprite sheet requires a non-zero texture width");
            AIKO_ASSERT(m_textureHeight > 0, "Sprite sheet requires a non-zero texture height");
            AIKO_ASSERT(m_frameWidth > 0, "Sprite sheet requires a non-zero frame width");
            AIKO_ASSERT(m_frameHeight > 0, "Sprite sheet requires a non-zero frame height");
            AIKO_ASSERT(m_frameWidth <= m_textureWidth, "Sprite sheet frame width exceeds texture width");
            AIKO_ASSERT(m_frameHeight <= m_textureHeight, "Sprite sheet frame height exceeds texture height");
            AIKO_ASSERT(m_textureWidth % m_frameWidth == 0, "Sprite sheet width must be divisible by frame width");
            AIKO_ASSERT(m_textureHeight % m_frameHeight == 0, "Sprite sheet height must be divisible by frame height");
        }

        uint32_t columns() const
        {
            return m_textureWidth / m_frameWidth;
        }

        uint32_t rows() const
        {
            return m_textureHeight / m_frameHeight;
        }

        uint32_t frameCount() const
        {
            return columns() * rows();
        }

        TextureRegion region(uint32_t column, uint32_t row) const
        {
            AIKO_ASSERT(column < columns(), "Sprite sheet column is out of bounds");
            AIKO_ASSERT(row < rows(), "Sprite sheet row is out of bounds");

            return TextureRegion::fromPixels(
                column * m_frameWidth,
                row * m_frameHeight,
                m_frameWidth,
                m_frameHeight,
                m_textureWidth,
                m_textureHeight);
        }

        TextureRegion region(uint32_t frameIndex) const
        {
            AIKO_ASSERT(frameIndex < frameCount(), "Sprite sheet frame index is out of bounds");

            const uint32_t column = frameIndex % columns();
            const uint32_t row = frameIndex / columns();

            return region(column, row);
        }

        uint32_t textureWidth() const { return m_textureWidth; }
        uint32_t textureHeight() const { return m_textureHeight; }
        uint32_t frameWidth() const { return m_frameWidth; }
        uint32_t frameHeight() const { return m_frameHeight; }

    private:

        uint32_t m_textureWidth = 0;
        uint32_t m_textureHeight = 0;
        uint32_t m_frameWidth = 0;
        uint32_t m_frameHeight = 0;
    };

}
