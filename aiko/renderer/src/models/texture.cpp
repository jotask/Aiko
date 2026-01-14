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
        this->create(1, 1);
    }

    void Texture::create(int width, int height)
    {
        backend->create(width, height);
    }

    void Texture::load(string file_path)
    {
        backend->load(file_path);
    }

    void Texture::unload()
    {
        backend->unload();
    }

}
