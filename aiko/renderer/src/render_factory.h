#pragma once

#include "models/shader.h"
#include "models/texture.h"

namespace aiko
{
    namespace renderer
    {
        class RendererFactory
        {
        public:
            static AikoPtr<interfaces::IShaderImpl> createShaderImpl();
            static AikoPtr<interfaces::ITextureImpl> createTextureImpl();
        };
    }
}