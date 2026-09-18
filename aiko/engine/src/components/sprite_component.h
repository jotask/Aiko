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

        size_t getWidth() const { return m_width; }
        size_t getHeight() const { return m_height; }

    protected:
        virtual void init() override;

    private:

        void resolveAssetBinding(AssetBindingContext& context) override;

        AssetReference<TextureAsset> m_texture;
        TextureRegion m_textureRegion = TextureRegion::full();

        std::optional<TextureAsset> m_pendingTexture;
        bool m_refreshRequested = false;

        Material m_material;

        size_t m_width = 0;
        size_t m_height = 0;

        vector<Color> pixels;
        bool is_dirty = false;

    };

}