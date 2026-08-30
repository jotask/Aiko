#include "sprite_component.h"

#include "models/mesh_factory.h"
#include "assets/types/shader_asset.h"

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
        m_pendingTexture.reset();

        m_texture.request(std::move(path));
        m_refreshRequested = false;

        m_width = 0;
        m_height = 0;
        pixels.clear();
        is_dirty = false;

        markAssetBindingDirty();
    }

    void SpriteComponent::load(TextureAsset texture)
    {
        AIKO_ASSERT(texture.desc.width > 0 && texture.desc.height > 0, "Sprite texture size must be greater than zero");
        AIKO_ASSERT(texture.pixels.size() == static_cast<size_t>(texture.desc.width) * static_cast<size_t>(texture.desc.height), "Sprite texture pixel count does not match texture size");

        m_texture.reset();
        m_refreshRequested = false;

        m_width = texture.desc.width;
        m_height = texture.desc.height;
        pixels = texture.pixels;

        m_pendingTexture = std::move(texture);
        is_dirty = false;

        markAssetBindingDirty();
    }

    void SpriteComponent::refresh()
    {
        if (is_dirty == false)
        {
            return;
        }

        m_refreshRequested = true;
        markAssetBindingDirty();

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

    const AssetId& SpriteComponent::getTextureId() const
    {
        return m_texture.isReady() ? m_texture.id() : InvalidAssetId;
    }

    bool SpriteComponent::resolveAssetBinding(AssetBindingContext& context)
    {
        if (m_texture.isRequested())
        {
            const AssetId textureId = context.load<TextureAsset>(m_texture.source());

            if (textureId == InvalidAssetId)
            {
                m_texture.fail();
                return false;
            }

            m_texture.markLoading(textureId);
            context.loadAsset<TextureAsset>(textureId);

            return true;
        }

        if (m_texture.isLoading())
        {
            const AssetId& textureId = m_texture.id();

            if (context.isLoaded<TextureAsset>(textureId) == false)
            {
                return true;
            }

            m_texture.resolve(textureId);
            m_material.diffuseTextureId = textureId;

            if (m_meshId == InvalidAssetId)
            {
                m_meshId = context.create(mesh::factory::generateQuad());
            }

            m_material.shaderId = context.load<ShaderAsset>("model");
        }

        if (m_pendingTexture.has_value())
        {
            if (m_meshId == InvalidAssetId)
            {
                m_meshId = context.create(mesh::factory::generateQuad());
            }

            m_material.shaderId = context.load<ShaderAsset>("model");

            m_pendingTexture->pixels = pixels;

            const AssetId textureId = context.create(*m_pendingTexture);

            m_texture.set(textureId);
            m_material.diffuseTextureId = textureId;

            m_pendingTexture.reset();
        }

        if (m_refreshRequested)
        {
            const AssetId& textureId = m_texture.id();

            if (textureId == InvalidAssetId)
            {
                m_refreshRequested = false;
                return false;
            }

            TextureAsset& textureAsset = context.getMutableTexture(textureId);

            textureAsset.pixels = pixels;
            textureAsset.desc.width = static_cast<uint>(m_width);
            textureAsset.desc.height = static_cast<uint>(m_height);

            context.invalidateTexture(textureId);

            is_dirty = false;
            m_refreshRequested = false;

        }
        return false;
    }

}
