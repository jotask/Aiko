 #pragma once

#include <vector>

#include "aiko_types.h"
#include "types/camera_types.h"
#include "types/color.h"
#include "shared/math.h"
#include "models/shader.h"

namespace aiko
{

    class Material
    {
    public:

        enum class MaterialType
        {

        };

        friend class RenderModule;
        friend class RenderSystem;

        Material();
        ~Material() = default;

    public:
        aiko::AikoPtr<Shader> m_shader;

    };

}
