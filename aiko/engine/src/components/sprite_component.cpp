#include "sprite_component.h"

#include <algorithm>

#include "models/mesh_factory.h"

#include "assets/asset_manager.h"
#include "models/game_object.h"

#include "systems/render_system.h"

namespace aiko
{

    SpriteComponent::SpriteComponent()
        : Component("Sprite")
    {

    }

    void SpriteComponent::init()
    {
    }

    void SpriteComponent::load(string file)
    {
        AIKO_ASSERT(file.empty() == false, "Attempting to load empty file");
        IComponentAssetAccess* assets = gameobject->getAiko()->getComponentAssetAccess();
        m_meshId = assets->registerMesh(mesh::factory::generateQuad());
        m_material.diffuseTextureId = assets->registerTexture(file);
        m_material.shaderId = assets->registerShader("model");
        is_dirty = false;
        pixels.clear();
    }

    void SpriteComponent::create(size_t width, size_t height)
    {
        IComponentAssetAccess* assets = gameobject->getAiko()->getComponentAssetAccess();
        m_meshId = assets->registerMesh(aiko::mesh::factory::generateQuad());
        m_material.shaderId = assets->registerShader("model");
        m_width = width;
        m_height = height;
        is_dirty = true;

        const size_t pixel_size = width * height;
        pixels.clear();
        pixels.resize(pixel_size, RAYWHITE);

        TextureAsset textureAsset{};
        textureAsset.desc.type = TextureType::Sampled;
        textureAsset.desc.format = TextureFormat::RGBA8;
        textureAsset.desc.width = static_cast<uint>(width);
        textureAsset.desc.height = static_cast<uint>(height);
        textureAsset.desc.mipmaps = 1;
        textureAsset.desc.computeWrite = false;
        textureAsset.pixels = pixels;

        m_material.diffuseTextureId = assets->registerTexture(textureAsset);

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

    void SpriteComponent::refresh()
    {
        if (is_dirty == false)
        {
            return;
        }
        AIKO_ASSERT(m_material.diffuseTextureId != InvalidAssetId, "SpriteComponent has no texture asset id");

        IComponentAssetAccess* assets = gameobject->getAiko()->getComponentAssetAccess();
        TextureAsset& textureAsset = assets->getMutableTextureAsset(m_material.diffuseTextureId);

        textureAsset.pixels = pixels;
        textureAsset.desc.width = static_cast<uint>(m_width);
        textureAsset.desc.height = static_cast<uint>(m_height);

        is_dirty = false;

        // Invalidate runtime texture cache so the renderer recreates it from updated CPU asset data.
        IRenderResourceInvalidator* invalidator = gameobject->getAiko()->getResourceInvalidator();
        AIKO_ASSERT(invalidator != nullptr, "No RenderSystem available");
        invalidator->invalidateTexture(m_material.diffuseTextureId);

    }

}
