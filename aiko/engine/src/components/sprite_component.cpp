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
        m_texture.request(std::move(path));

        m_createRequested = false;
        m_refreshRequested = false;

        m_width = 0;
        m_height = 0;
        pixels.clear();
        is_dirty = false;

        markAssetBindingDirty();
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

    void SpriteComponent::resolveAssetBinding(AssetBindingContext& context)
    {
        if (m_texture.isRequested())
        {
            m_texture.markLoading();

            const AssetId textureId = context.load<TextureAsset>(m_texture.source());

            if (textureId == InvalidAssetId)
            {
                m_texture.fail();
            }
            else
            {
                m_texture.resolve(textureId);
                m_material.diffuseTextureId = textureId;
            }

            if (m_meshId == InvalidAssetId)
            {
                m_meshId = context.create(mesh::factory::generateQuad());
            }

            m_material.shaderId = context.load<ShaderAsset>("model");
        }

        if (m_createRequested)
        {
            if (m_meshId == InvalidAssetId)
            {
                m_meshId = context.create(mesh::factory::generateQuad());
            }

            m_material.shaderId = context.load<ShaderAsset>("model");

            TextureAsset textureAsset{};
            textureAsset.desc.type = TextureType::Sampled;
            textureAsset.desc.format = TextureFormat::RGBA8;
            textureAsset.desc.width = static_cast<uint>(m_width);
            textureAsset.desc.height = static_cast<uint>(m_height);
            textureAsset.desc.mipmaps = 1;
            textureAsset.desc.computeWrite = false;
            textureAsset.pixels = pixels;

            const AssetId textureId = context.create(textureAsset);

            setTextureId(textureId);

            m_createRequested = false;
            is_dirty = false;
        }

        if (m_refreshRequested)
        {
            const AssetId& textureId = m_texture.id();

            if (textureId == InvalidAssetId)
            {
                m_refreshRequested = false;
                return;
            }

            TextureAsset& textureAsset = context.getMutableTexture(textureId);

            textureAsset.pixels = pixels;
            textureAsset.desc.width = static_cast<uint>(m_width);
            textureAsset.desc.height = static_cast<uint>(m_height);

            context.invalidateTexture(textureId);

            is_dirty = false;
            m_refreshRequested = false;
        }
    }

}
