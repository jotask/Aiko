#include "texture.h"

#include "render_factory.h"

namespace aiko
{

    Texture::Texture()
        : backend(renderer::RendererFactory::createTextureImpl())
    {

    }

    void Texture::use()
    {
        backend->use();
    }

    void Texture::unuse()
    {
        backend->unuse();
    }

    bool Texture::isValid() const
    {
        return backend->isValid();
    }

    RenderResourceId Texture::id() const
    {
        return backend->id();
    }

    TextureInfo Texture::getInfo() const
    {
        return backend->getInfo();
    }

    void Texture::create()
    {
        const TextureDesc text
        {
            .type = TextureType::Sampled,
            .format = TextureFormat::RGBA8,
            .width = 1,
            .height = 1,
            .mipmaps = 1,
            .computeWrite = false,
        };
        this->create(text);
    }

    void Texture::create(int width, int height)
    {
        const TextureDesc text
        {
            .type = TextureType::Sampled,
            .format = TextureFormat::RGBA8,
            .width = width,
            .height = height,
            .mipmaps =  1,
            .computeWrite = false,
        };
        this->create(text);
    }

    void Texture::create(const TextureDesc& text)
    {
        backend->create(text);
    }

    void Texture::upload(const TextureAsset& asset)
    {
        create(asset.desc);
        setPixels(asset.pixels);
    }

    void Texture::update(const TextureAsset& asset)
    {
        if (!isValid())
        {
            upload(asset);
            return;
        }

        const TextureInfo info = getInfo();
        if (info.width != asset.desc.width ||
            info.height != asset.desc.height ||
            info.format != asset.desc.format)
        {
            unload();
            upload(asset);
            return;
        }

        backend->update(asset);
    }

    void Texture::unload()
    {
        backend->unload();
    }

    void Texture::setPixels(const vector<Color>& pixels)
    {
        backend->setPixels(pixels);
    }
}
