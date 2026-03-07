#pragma once

#include <aiko_types.h>
#include "assets/asset_id.h"
#include "assets/types/mesh_asset.h"

#include "models/component.h"

namespace aiko
{

    class SpriteComponent : public Component
    {

        // Scene/reference sprite component.
        // Stores CPU-side sprite data and material references.
        // Does not own runtime GPU resources directly.

    public:

        SpriteComponent(size_t, size_t);
        SpriteComponent(string files);
        virtual ~SpriteComponent() = default;

        virtual void init() override;

        void load(string);
        void create(size_t width, size_t height);

        // TODO TMP for now
        void setPixel(size_t x, size_t y, Color c);
        void setPixels(std::vector<Color> pixels);
        void refresh();

        const AssetId& getMeshId() const { return m_meshId; }
        const MaterialAsset& getMaterial() const { return m_material; }

    private:
        AssetId             m_meshId;
        MaterialAsset       m_material;

        size_t m_width = 0;
        size_t m_height = 0;

        std::vector<Color> pixels;
        bool is_dirty = false;

    };

}