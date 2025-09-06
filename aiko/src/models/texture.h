 #pragma once

#include <vector>

#include "aiko_types.h"
#include "types/camera_types.h"
#include "types/color.h"
#include "shared/math.h"
#include "models/shader.h"
#include "types/textures.h"

namespace aiko
{
    class RenderModule;
    class Texture
    {
    private:
        friend class RenderModule;
        static RenderModule* s_renderModule;
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

    //private:

        texture::Texture m_texture;

    };

}
