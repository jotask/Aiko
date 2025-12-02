#pragma once

#include "models/shader.h"
#include "models/texture.h"
#include "models/mesh.h"

namespace aiko
{
    namespace renderer
    {
        class RendererFactory
        {
        public:
            static AikoPtr<interfaces::IShaderImpl> createShaderImpl();
            static AikoPtr<interfaces::ITextureImpl> createTextureImpl();
            static AikoPtr<interfaces::IMeshImpl> createMeshImpl(Mesh*);
        };
    }
}