#pragma once

#include <aiko_types.h>

#include "types/color.h"
#include "models/material.h"
#include "models/mesh.h"
#include "models/screen_fbo.h"

namespace aiko::renderer
{

    using ViewId = uint16_t;

    struct DeviceInitDesc
    {
        void* nativeWindowHandle = nullptr;
        u32 width = 0;
        u32 height = 0;
        bool vsync = false;
    };

    struct PassDescription
    {
        u32 width;
        u32 height;
        bool clearColor = true;
        bool clearDepth = true;
        Color clear = BLACK;
    };

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

        virtual void setViewTransform(ViewId viewId, const mat4& view, const mat4& projection) = 0;
        virtual void renderMesh(ViewId viewId, const mat4 world, const Mesh& mesh, const Material& material) = 0;

        virtual void bindMaterial(const Material& material) = 0;
        virtual void drawMesh(ViewId viewId, const mat4& world, const Mesh& mesh, const Material& material) = 0;

        virtual void presentFrameBufferToScreen(ViewId viewId, const ScreenFbo& fb) = 0;

        virtual void drawMeshInstanced(ViewId viewId, const Mesh& mesh, const Material& material, const void* data, u32 instanceCount, u32 instanceStrideBytes) = 0;

    };
}