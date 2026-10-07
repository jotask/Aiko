#pragma once

#include <optional>

#include "assets/asset_binding.h"
#include "assets/asset_reference.h"
#include "assets/types/texture_asset.h"
#include <models/texture_region.h>
#include <types/color.h>

#include "models/component.h"
#include "ui/ui_raycast.h"

namespace aiko
{

    class ImageComponent : public Component, public IAssetBinding, public UIRaycastTarget
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
            m_colorOverride = color;
        }

        void clearColorOverride()
        {
            m_colorOverride.reset();
        }

        bool hasColorOverride() const
        {
            return m_colorOverride.has_value();
        }

        Color getColorOverride() const
        {
            AIKO_ASSERT(m_colorOverride.has_value(), "ImageComponent has no color override");
            return *m_colorOverride;
        }

    private:

        void resolveAssetBinding(AssetBindingContext& context) override;

        AssetReference<TextureAsset> m_texture;

        TextureRegion m_textureRegion = TextureRegion::full();

        std::optional<Color> m_colorOverride;
    };

}
