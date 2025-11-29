#pragma once

#include "models/shader.h"

namespace aiko
{
    namespace renderer
    {
        class RendererFactory
        {
        public:
            static AikoPtr<interfaces::IShaderImpl> createShaderImpl();
        };
    }
}