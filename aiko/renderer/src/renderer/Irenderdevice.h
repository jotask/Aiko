#pragma once

#include <aiko_types.h>

#include "models/compute_shader.h"
#include "models/material.h"
#include "models/mesh.h"
#include "models/screen_fbo.h"
#include "types/render_types.h"
#include "renderer/frame_uniforms.h"

namespace aiko::renderer
{

    class IRenderDevice
    {
    public:
        IRenderDevice() = default;
        virtual ~IRenderDevice() = default;

        virtual bool init(const DeviceInitDesc& desc) = 0;
        virtual void shutdown() = 0;

        virtual void resize(u32 width, u32 height, bool vsync) = 0;

        virtual void beginFrame() = 0;
        virtual void endFrame() = 0;

        virtual void beginPass(ViewId viewId, const PassDescription& pass, FrameBuffer* frameBuffer = nullptr) = 0;
        virtual void endPass() = 0;

        virtual void present() = 0;

        virtual void renderMesh(ViewId viewId, const mat4 world, const Mesh& mesh, const Material& material) = 0;

        virtual void bindMaterial(const Material& material) = 0;
        virtual void drawMesh(ViewId viewId, const mat4& world, const Mesh& mesh, const Material& material) = 0;

        virtual void presentFrameBufferToScreen(ViewId viewId, const ScreenFbo& fb) = 0;

        virtual void drawMeshInstanced(ViewId viewId, const Mesh& mesh, const Material& material, const void* data, u32 instanceCount, u32 instanceStrideBytes) = 0;

        virtual void bindFrame(ViewId viewId, const FrameData& u) = 0;

        // Compute Shader
        virtual void setComputeImage(ViewId viewId, const Texture& texture, ComputeAccess access) = 0;
        virtual void dispatch( ViewId viewId, const ComputeShader& program, uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ) = 0;

    };
}