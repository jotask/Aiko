#include "texture.h"

namespace aiko
{

    Texture::Texture()
        : m_texture({0})
    {

    }

    void Texture::loadTextureFromFile(const char* file)
    {

    }

    bool Texture::isValid() const
    {
        return m_texture.id != 0;
    };

}
