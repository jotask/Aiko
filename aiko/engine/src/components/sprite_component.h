#pragma once

#include <aiko_types.h>
#include "assets/asset_id.h"
#include "assets/types/mesh_asset.h"
#include "metadata/material_instance.h"

#include "models/component.h"

namespace aiko
{

    class SpriteComponent : public Component
    {

        // Scene/reference sprite component.
        // Stores CPU-side sprite data and material references.
        // Does not own runtime GPU resources directly.

    public:

        SpriteComponent();
        virtual ~SpriteComponent() = default;

        virtual void init() override;

        void load(string);
        void create(size_t width, size_t height);

        // TODO TMP for now
        void setPixel(size_t x, size_t y, Color c);
        void setPixels(std::vector<Color> pixels);
        void refresh();

        MaterialAsset& getMaterial() { return m_material; }
        const MaterialAsset& getMaterial() const { return m_material; }

        MaterialInstance& getMaterialInstance() { return m_materialInstance; }
        const MaterialInstance& getMaterialInstance() const { return m_materialInstance; }

        void setTextureId(const AssetId& id) { m_material.diffuseTextureId = id; }
        const AssetId& getTextureId() const { return m_material.diffuseTextureId; }

        const AssetId& getMeshId() const { return m_meshId; }
        size_t getWidth() const { return m_width; }
        size_t getHeight() const { return m_height; }

    private:
        AssetId             m_meshId = InvalidAssetId;
        MaterialAsset       m_material;
        MaterialInstance m_materialInstance;

        size_t m_width = 0;
        size_t m_height = 0;

        std::vector<Color> pixels;
        bool is_dirty = false;

    };

}