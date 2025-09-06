#include "texture.h"

#include <stdexcept>

#include "modules/render/render_module.h"

namespace aiko
{

    RenderModule* Texture::s_renderModule = nullptr;
    
    Texture::Texture()
        : m_texture({0})
    {
        // s_renderModule->createTexture();
    }

    void Texture::loadTextureFromFile(const char* file)
    {
        m_texture = s_renderModule->loadTexture(file);
    }

}
