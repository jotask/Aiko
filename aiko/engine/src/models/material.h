 #pragma once

#include <vector>

#include "aiko_types.h"
#include "time/shader.h"
#include "time/texture.h"

 namespace aiko
{

    class Material
    {
    public:

        friend class RenderModule;
        friend class RenderSystem;

        Material();
        ~Material() = default;

    public:
        Shader m_shader;

		Color m_baseColor;

        Texture m_diffuse;

    };

}
