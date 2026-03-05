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

    uint Texture::id() const
    {
        return backend->id();
    }

    texture::Texture Texture::getInfo() const
    {
        return backend->getInfo();
    }

    void Texture::create()
    {
        const texture::Texture text
        {
            .type = texture::TextureType::Sampled,
            .format = texture::TextureFormat::RGBA8,
            .width = 1,
            .height = 1,
            .mipmaps =  false,
        };
        this->create(text);
    }

    void Texture::create(int width, int height)
    {
        const texture::Texture text
        {
            .type = texture::TextureType::Sampled,
            .format = texture::TextureFormat::RGBA8,
            .width = width,
            .height = height,
            .mipmaps =  false,
        };
        this->create(text);
    }

    void Texture::create(texture::Texture text)
    {
        backend->create(text);
    }

    void Texture::load(string file_path)
    {
        backend->load(file_path);
    }

    void Texture::unload()
    {
        backend->unload();
    }

    void Texture::setPixels(std::vector<Color>& pixels)
    {
        backend->setPixels(pixels);
    }
}
