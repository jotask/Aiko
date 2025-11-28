 #pragma once

#include "types/textures.h"

namespace aiko
{
    class RenderModule;
    class Texture
    {
    public:

        enum class MeshType
        {
            TEST,
            QUAD,
            CUSTOM,
        };

        friend class RenderModule;
        friend class RenderSystem;
    
        Texture();
        ~Texture() = default;


        void loadTextureFromFile(const char*);

		bool isValid() const;

    //private:

        texture::Texture m_texture;

    };

}
