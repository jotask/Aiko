#pragma once

#include "assets/asset_binding.h"
#include "assets/asset_id.h"
#include "assets/asset_reference.h"
#include "assets/types/texture_asset.h"
#include "models/material.h"
#include "models/component.h"
#include "models/texture_region.h"

#include <aiko_types.h>

#include <optional>

namespace aiko
{

    class SpriteComponent : public Component, public IAssetBinding
    {
    public:

        SpriteComponent();
        virtual ~SpriteComponent() = default;

        void load(string path);
        void load(TextureAsset texture);
        void refresh();

        // TODO TMP for now
        void setPixel(size_t x, size_t y, Color c);
        void setPixels(const vector<Color>& pixels);

        bool isDirty() const { return is_dirty; }
        const vector<Color>& getPixels() const { return pixels; }

        Material& getMaterial() { return m_material; }
        const Material& getMaterial() const { return m_material; }

        const AssetId& getTextureId() const;

        void setTextureRegion(const TextureRegion& region) { m_textureRegion = region; }
        const TextureRegion& getTextureRegion() const { return m_textureRegion; }

        void setSize(const vec2& size)
        {
            AIKO_ASSERT(size.x > 0.0f, "Sprite width must be greater than zero");
            AIKO_ASSERT(size.y > 0.0f, "Sprite height must be greater than zero");

            m_size = size;
        }

        const vec2& getSize() const { return m_size; }

        void setPivot(const vec2& pivot)
        {
            AIKO_ASSERT(pivot.x >= 0.0f && pivot.x <= 1.0f, "Sprite pivot x must be between zero and one");
            AIKO_ASSERT(pivot.y >= 0.0f && pivot.y <= 1.0f, "Sprite pivot y must be between zero and one");

            m_pivot = pivot;
        }

        const vec2& getPivot() const { return m_pivot; }

        void setFlipX(bool flip) { m_flipX = flip; }
        void setFlipY(bool flip) { m_flipY = flip; }

        bool getFlipX() const { return m_flipX; }
        bool getFlipY() const { return m_flipY; }

        size_t getWidth() const { return m_width; }
        size_t getHeight() const { return m_height; }

    protected:
        virtual void init() override;

    private:

        void resolveAssetBinding(AssetBindingContext& context) override;

        AssetReference<TextureAsset> m_texture;
        TextureRegion m_textureRegion = TextureRegion::full();

        vec2 m_size = {1.0f, 1.0f};
        vec2 m_pivot = {0.5f, 0.5f};
        bool m_flipX = false;
        bool m_flipY = false;

        std::optional<TextureAsset> m_pendingTexture;
        bool m_refreshRequested = false;

        Material m_material;

        size_t m_width = 0;
        size_t m_height = 0;

        vector<Color> pixels;
        bool is_dirty = false;

    };

}