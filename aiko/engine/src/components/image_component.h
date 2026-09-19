#pragma once

#include <assets/asset_id.h>
#include <models/texture_region.h>
#include <types/color.h>

#include "models/component.h"

namespace aiko
{

    class ImageComponent : public Component
    {
    public:

        ImageComponent();
        virtual ~ImageComponent() = default;

        void setTexture(AssetId textureId)
        {
            m_textureId = textureId;
        }

        const AssetId& getTexture() const
        {
            return m_textureId;
        }

        void setTextureRegion(const TextureRegion& region)
        {
            m_textureRegion = region;
        }

        const TextureRegion& getTextureRegion() const
        {
            return m_textureRegion;
        }

        void setColor(Color color)
        {
            m_color = color;
        }

        Color getColor() const
        {
            return m_color;
        }

    private:

        AssetId m_textureId = InvalidAssetId;
        TextureRegion m_textureRegion = TextureRegion::full();

        Color m_color = WHITE;
    };

}
