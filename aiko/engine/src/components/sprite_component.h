#pragma once

#include "assets/asset_binding.h"
#include "assets/asset_id.h"
#include "assets/asset_reference.h"
#include "assets/types/mesh_asset.h"
#include "assets/types/texture_asset.h"
#include "metadata/material_instance.h"
#include "models/component.h"

#include <aiko_types.h>

namespace aiko
{

    class SpriteComponent : public Component, public IAssetBinding
    {
    public:

        SpriteComponent();
        virtual ~SpriteComponent() = default;

        virtual void init() override;

        void load(string path);
        void create(size_t width, size_t height);
        void refresh();

        // TODO TMP for now
        void setPixel(size_t x, size_t y, Color c);
        void setPixels(const vector<Color>& pixels);

        bool isDirty() const { return is_dirty; }
        const vector<Color>& getPixels() const { return pixels; }

        void setSize(size_t width, size_t height)
        {
            m_width = width;
            m_height = height;
            pixels.clear();
            pixels.resize(width * height, RAYWHITE);
            is_dirty = true;
        }

        MaterialAsset& getMaterial() { return m_material; }
        const MaterialAsset& getMaterial() const { return m_material; }

        MaterialInstance& getMaterialInstance() { return m_materialInstance; }
        const MaterialInstance& getMaterialInstance() const { return m_materialInstance; }

        const AssetId& getTextureId() const { return m_texture.id(); }

        const AssetId& getMeshId() const { return m_meshId; }
        size_t getWidth() const { return m_width; }
        size_t getHeight() const { return m_height; }

    private:

        void resolveAssetBinding(AssetBindingContext& context) override;

        AssetReference<TextureAsset> m_texture;

        bool m_createRequested = false;
        bool m_refreshRequested = false;

        AssetId             m_meshId = InvalidAssetId;
        MaterialAsset       m_material;
        MaterialInstance m_materialInstance;

        size_t m_width = 0;
        size_t m_height = 0;

        vector<Color> pixels;
        bool is_dirty = false;

    };

}