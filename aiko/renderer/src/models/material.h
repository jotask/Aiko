 #pragma once

#include "models/shader.h"
#include "models/texture.h"
#include "types/color.h"

 namespace aiko
{

    class Material
    {
    public:

        friend class RenderModule;
        friend class RenderSystem;

        Material();
        ~Material() = default;

        u64 id() const;

    public:
        Shader m_shader;

        bool m_userVertexColor;
        bool m_lit;

		Color m_baseColor;
        Texture m_diffuse;

    };

}
