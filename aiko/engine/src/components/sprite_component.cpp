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
        m_material.m_shader.load("model");
        AIKO_ASSERT(m_material.m_shader.isValid(), "Shader is invalid");
        const Mesh::MeshData data = mesh::factory::generateQuad();
        m_mesh.setData(data);
        AIKO_ASSERT(m_mesh.isValid(), "Mesh is invalid");
    }

    void SpriteComponent::load(string file)
    {
        AIKO_ASSERT(file.empty() == false, "Attempting to load empty file")
        if (m_material.m_diffuse.isValid() == true)
        {
            m_material.m_diffuse.unload();
        }
        m_material.m_diffuse.load(file);
        AIKO_ASSERT(m_material.m_diffuse.isValid(), "Texture is invalid");
        is_dirty = false;
        const auto info = m_material.m_diffuse.getInfo();
        const size_t pixel_size = info.width * info.height;
        pixels.reserve(pixel_size);
        pixels.resize(pixel_size);
        std::fill(pixels.begin(), pixels.end(), RAYWHITE);
    }

    void SpriteComponent::create(size_t width, size_t height)
    {
        m_material.m_diffuse.create(width, height);
        AIKO_ASSERT(m_material.m_diffuse.isValid(), "Invalid texture");
        is_dirty = true;
        const size_t pixel_size = width * height;
        pixels.reserve(pixel_size);
        pixels.resize(pixel_size);
        std::fill(pixels.begin(), pixels.end(), RAYWHITE);
    }

    void SpriteComponent::setPixel(size_t x, size_t y, Color c)
    {
        const auto info = m_material.m_diffuse.getInfo();
        const size_t index = y * info.width + x;
        AIKO_ASSERT(x < info.width && y < info.height, "OOB pixel coords");
        AIKO_ASSERT(pixels.size() == size_t(info.width) * info.height, "Pixel buffer mismatch");
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
        pixels.clear();
        pixels.insert(pixels.end(), ps.begin(), ps.end());
        is_dirty = true;
    }

    void SpriteComponent::refresh()
    {
        if (is_dirty == false)
        {
            return;
        }
        is_dirty = false;
        m_material.m_diffuse.setPixels(pixels);
    }

    Mesh& SpriteComponent::getMesh()
    {
        return m_mesh;
    }

    Material& SpriteComponent::getMaterial()
    {
        return m_material;
    }

}
