#include "sprite_component.h"

#include <algorithm>

namespace aiko
{

    SpriteComponent::SpriteComponent()
        : Component("Sprite")
    {

    }

    void SpriteComponent::init()
    {
    }

    void SpriteComponent::load(string path)
    {
        m_texture.request(std::move(path));

        m_createRequested = false;
        m_refreshRequested = false;

        m_width = 0;
        m_height = 0;
        pixels.clear();
        is_dirty = false;
    }

    void SpriteComponent::create(size_t width, size_t height)
    {
        AIKO_ASSERT(width > 0 && height > 0, "Sprite size must be greater than zero");

        m_width = width;
        m_height = height;

        pixels.clear();
        pixels.resize(width * height, RAYWHITE);

        m_createRequested = true;
        m_refreshRequested = false;
        is_dirty = true;
    }

    void SpriteComponent::refresh()
    {
        if (is_dirty == false)
        {
            return;
        }

        m_refreshRequested = true;
    }

    void SpriteComponent::setPixel(size_t x, size_t y, Color c)
    {
        AIKO_ASSERT(x < m_width && y < m_height, "OOB pixel coords");
        const size_t index = y * m_width + x;
        AIKO_ASSERT(index < pixels.size(), "Pixel buffer mismatch");

        if (pixels[index] == c)
        {
            return;
        }

        pixels[index] = c;
        is_dirty = true;
    }

    void SpriteComponent::setPixels(const vector<Color>& ps)
    {
        AIKO_ASSERT(pixels.size() == ps.size(), "New pixels don't match texture size");
        std::ranges::copy(ps, pixels.begin());
        is_dirty = true;
    }

}
