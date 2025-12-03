#pragma once

#include "models/shader.h"
#include "models/texture.h"
#include "models/frame_buffer.h"
#include "models/screen_fbo.h"
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
            static AikoPtr<interfaces::IFrameBufferImpl> createFrameBufferImpl();
            static AikoPtr<interfaces::IScreenFboImpl> createScreenFboImpl();
            static AikoPtr<interfaces::IMeshImpl> createMeshImpl(Mesh*);
        };
    }
}