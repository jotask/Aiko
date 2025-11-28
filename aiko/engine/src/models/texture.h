 #pragma once

#include <vector>

#include <aiko_types.h>
#include <math/math.h>

#include "types/camera_types.h"
#include "types/color.h"
#include "models/shader.h"
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
