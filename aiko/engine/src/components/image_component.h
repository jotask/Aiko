#pragma once

#include "assets/asset_binding.h"
#include "assets/asset_reference.h"
#include "assets/types/texture_asset.h"
#include <models/texture_region.h>
#include <types/color.h>

#include "models/component.h"

namespace aiko
{

    class ImageComponent : public Component, public IAssetBinding
    {
    public:

        ImageComponent();
        virtual ~ImageComponent() = default;

        void load(string path);

        const AssetId& getTextureId() const;
        bool hasTexture() const;

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

        void resolveAssetBinding(AssetBindingContext& context) override;

        AssetReference<TextureAsset> m_texture;

        TextureRegion m_textureRegion = TextureRegion::full();

        Color m_color = WHITE;
    };

}
