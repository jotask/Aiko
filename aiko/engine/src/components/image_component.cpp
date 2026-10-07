#include "image_component.h"

#include <utility>

namespace aiko
{

    ImageComponent::ImageComponent()
        : Component("Image")
    {
    }

    void ImageComponent::load(string path)
    {
        m_texture.request(std::move(path));
    }

    const AssetId& ImageComponent::getTextureId() const
    {
        return m_texture.isReady()
            ? m_texture.id()
            : InvalidAssetId;
    }

    bool ImageComponent::hasTexture() const
    {
        return m_texture.isRequested() ||
               m_texture.isLoading() ||
               m_texture.isReady();
    }

    void ImageComponent::resolveAssetBinding(AssetBindingContext& context)
    {
        if (m_texture.isRequested())
        {
            const AssetId textureId =
                context.load<TextureAsset>(m_texture.source());

            if (textureId == InvalidAssetId)
            {
                m_texture.fail();
                return;
            }

            m_texture.markLoading(textureId);
            context.loadAsset<TextureAsset>(textureId);
            return;
        }

        if (m_texture.isLoading())
        {
            const AssetId& textureId = m_texture.id();

            if (context.isLoaded<TextureAsset>(textureId) == false)
            {
                return;
            }

            m_texture.resolve(textureId);
        }
    }

}
