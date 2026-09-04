#pragma once

#include <aiko_types.h>

#include "models/compute_buffer.h"
#include "models/compute_shader.h"
#include "models/material.h"
#include "models/mesh.h"
#include "models/texture.h"
#include "models/screen_fbo.h"
#include "models/shader.h"
#include "models/frame_buffer.h"
#include "types/render_types.h"
#include "renderer/frame_uniforms.h"
#include "types/compute_pass.h"
#include "types/draw_types.h"
#include "types/transient_types.h"

namespace aiko
{
    class RenderResourceManager;
}

namespace aiko::renderer
{

    class IRenderDevice
    {
    public:
        IRenderDevice(RenderResourceManager* manager) : m_resource_manager(manager) {};
        virtual ~IRenderDevice() = default;

        virtual bool init(const DeviceInitDesc& desc) = 0;
        virtual void shutdown() = 0;
        virtual void waitIdle() {};

        virtual void resize(u32 width, u32 height, bool vsync) = 0;

        virtual void beginFrame() = 0;
        virtual void endFrame() = 0;

        virtual void beginPass(ViewId viewId, const PassDescription& pass, const FrameBuffer* frameBuffer = nullptr) = 0;
        virtual void endPass() = 0;

        virtual void present() = 0;

    protected:
        virtual void bindMaterial(const Material& material) = 0;
        virtual void drawMesh(ViewId viewId, const mat4& world, const Mesh& mesh, const Material& material) = 0;
        virtual void drawMeshInstanced(ViewId viewId, const Mesh& mesh, const Material& material, const void* data, u32 instanceCount, u32 instanceStrideBytes) = 0;
        virtual void drawTransient(ViewId viewId, const TransientDrawDesc& desc) = 0;

    public:

        virtual void presentFrameBufferToScreen(ViewId viewId, const ScreenFbo& fb) = 0;
        virtual void presentTextureToScreen(ViewId viewId, const ScreenFbo& screen, const Texture& texture) = 0;

        virtual void bindFrame(ViewId viewId, const FrameData& u) = 0;

        // Compute Shader
        virtual void execute(ViewId viewId, const ComputePass& pass) = 0;
        virtual void requestReadback(const ComputeReadbackRequest& request) = 0;
        virtual bool pollReadback(ComputeReadbackResult& result) = 0;

        virtual void drawMeshInstancedGpu(ViewId viewId, const GpuInstanceDrawDesc& desc) = 0;
        virtual void drawBillboards(ViewId viewId, const GpuBillboardDrawDesc& desc) = 0;
        virtual void drawVerticesGpu(ViewId viewId, const GpuVertexDrawDesc& desc) = 0;

        void submitMesh(ViewId viewId, const MeshDrawPacket& packet)
        {
            AIKO_ASSERT(packet.mesh != nullptr, "MeshDrawPacket has null mesh");
            AIKO_ASSERT(packet.material != nullptr, "MeshDrawPacket has null material");

            bindMaterial(*packet.material);
            drawMesh(viewId, packet.world, *packet.mesh, *packet.material);
        }

        void submitInstanced(ViewId viewId, const InstancedDrawPacket& packet)
        {
            AIKO_ASSERT(packet.mesh != nullptr, "InstancedDrawPacket has null mesh");
            AIKO_ASSERT(packet.material != nullptr, "InstancedDrawPacket has null material");
            AIKO_ASSERT(packet.data != nullptr, "InstancedDrawPacket has null data");
            AIKO_ASSERT(packet.instanceCount > 0, "InstancedDrawPacket has zero instanceCount");
            AIKO_ASSERT(packet.stride > 0, "InstancedDrawPacket has zero stride");

            drawMeshInstanced(viewId, *packet.mesh, *packet.material, packet.data, packet.instanceCount, packet.stride);
        }

        void submitTransient(ViewId viewId, const TransientDrawDesc& desc)
        {
            AIKO_ASSERT(desc.material != nullptr, "TransientDrawDesc has null material");
            bindMaterial(*desc.material);
            drawTransient(viewId, desc);
        }

        void submitGpuInstanced(ViewId viewId, const GpuInstanceDrawDesc& desc)
        {
            drawMeshInstancedGpu(viewId, desc);
        }

        void submitGpuVertices(ViewId viewId, const GpuVertexDrawDesc& desc)
        {
            drawVerticesGpu(viewId, desc);
        }

        void submitBillboards(ViewId viewId, const GpuBillboardDrawDesc& desc)
        {
            drawBillboards(viewId, desc);
        }

        virtual void prepareTextureForSampling(const Texture& texture)
        {
            AIKO_UNUSED(texture);
        }

        virtual void prepareMaterial(const Material& material)
        {
            AIKO_UNUSED(material);
        }

        virtual void prepareGpuReadBuffers(const vector<GpuReadBufferBinding>& bindings)
        {
            AIKO_UNUSED(bindings);
        }

        virtual void prepareVertexBuffer(const ComputeBuffer& buffer)
        {
            AIKO_UNUSED(buffer);
        }

        virtual void prepareIndexBuffer(const ComputeBuffer& buffer)
        {
            AIKO_UNUSED(buffer);
        }

        virtual void prepareIndirectBuffer(const ComputeBuffer& buffer)
        {
            AIKO_UNUSED(buffer);
        }

    protected:

        RenderResourceManager* getResources() const { return m_resource_manager; }

        interfaces::ITextureImpl* getTextureBackend(const Texture& texture) const
        {
            return texture.backend.get();
        }

        interfaces::IFrameBufferImpl* getFrameBufferBackend(const FrameBuffer& frameBuffer) const
        {
            return frameBuffer.backend.get();
        }

        interfaces::IMeshImpl* getMeshBackend(const Mesh& mesh) const
        {
            return mesh.backend.get();
        }

        interfaces::IShaderImpl* getShaderBackend(const Shader& shader) const
        {
            return shader.backend.get();
        }

        interfaces::IComputeShaderImpl* getComputeShaderBackend(const ComputeShader& shader) const
        {
            return shader.backend.get();
        }

        interfaces::IComputeBufferImpl* getComputeBufferBackend(const ComputeBuffer& buffer) const
        {
            return buffer.backend.get();
        }

        AikoPtr<interfaces::IComputeBufferImpl> retainComputeBufferBackend(const ComputeBuffer& buffer) const
        {
            return buffer.backend;
        }

    private:

        RenderResourceManager* m_resource_manager;

    };

}
