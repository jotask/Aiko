#include "sprite_component.h"

#include "models/mesh_factory.h"

namespace aiko
{

    SpriteComponent::SpriteComponent(size_t width, size_t height)
        : Component("Sprite")
    {
        create(width, height);
    }

    SpriteComponent::SpriteComponent(string file)
        : Component("Sprite")
    {
        load(file);
    }

    void SpriteComponent::init()
    {
    }

    void SpriteComponent::load(string file)
    {
        AIKO_ASSERT(file.empty() == false, "Attempting to load empty file");
        m_material.diffuseTextureId = InvalidAssetId; // temporary until asset registration path is wired
        is_dirty = false;
        pixels.clear();
    }

    void SpriteComponent::create(size_t width, size_t height)
    {
        m_width = width;
        m_height = height;
        is_dirty = true;
        const size_t pixel_size = width * height;
        pixels.clear();
        pixels.resize(pixel_size, RAYWHITE);

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

    void SpriteComponent::setPixels(std::vector<Color> ps)
    {
        AIKO_ASSERT(pixels.size() == ps.size(), "New pixels don't match texture size");
        pixels = std::move(ps);
        is_dirty = true;
    }

    void SpriteComponent::refresh()
    {
        if (is_dirty == false)
        {
            return;
        }

        is_dirty = false;
    }

}
