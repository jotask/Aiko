#pragma once

#include "models/shader.h"
#include "models/texture.h"
#include "models/frame_buffer.h"
#include "models/compute_shader.h"
#include "models/mesh.h"
#include "renderer/Irenderdevice.h"

namespace aiko
{
    namespace renderer
    {
        class RendererFactory
        {
        public:
            static AikoPtr<IRenderDevice> createRenderDevice();

            static AikoPtr<interfaces::IShaderImpl> createShaderImpl();
            static AikoPtr<interfaces::ITextureImpl> createTextureImpl();
            static AikoPtr<interfaces::IFrameBufferImpl> createFrameBufferImpl();
            static AikoPtr<interfaces::IComputeShaderImpl> createComputeShaderImpl();
            static AikoPtr<interfaces::IMeshImpl> createMeshImpl(Mesh*);
        };
    }
}
