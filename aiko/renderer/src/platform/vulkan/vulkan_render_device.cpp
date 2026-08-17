#include "vulkan_render_device.h"

#include <math/math_vector.h>
#include <logger/logger.h>
#include <intrumentor/profiler.h>

#include "vulkan_platform_helper.h"
#include "display/display_manager.h"
#include "impl/vulkan_computebuffer_impl.h"
#include "impl/vulkan_computeshader_impl.h"
#include "impl/vulkan_texture_impl.h"

#include <platform/vulkan/impl/vulkan_shader_impl.h>
#include <platform/vulkan/impl/vulkan_mesh_impl.h>
#include <platform/vulkan/impl/vulkan_framebuffer_impl.h>

#include "resources/render_resource_manager.h"

namespace aiko::renderer::vulkan
{

    VulkanRenderDevice::VulkanRenderDevice(RenderResourceManager* resources)
        : IRenderDevice(resources)
    {

    }

    VulkanRenderDevice::~VulkanRenderDevice()
    {
    }

    bool VulkanRenderDevice::init(const DeviceInitDesc& desc)
    {
        return false;
    }

    void VulkanRenderDevice::shutdown()
    {

    }

    void VulkanRenderDevice::resize(u32 width, u32 height, bool vsync)
    {

    }

    void VulkanRenderDevice::beginFrame()
    {

    }

    void VulkanRenderDevice::endFrame()
    {

    }

    void VulkanRenderDevice::beginPass(uint16_t viewId, const PassDescription& pass, const FrameBuffer* frameBuffer)
    {

    }

    void VulkanRenderDevice::endPass()
    {
    }

    void VulkanRenderDevice::present()
    {

    }

    void VulkanRenderDevice::renderMesh(ViewId viewId, const mat4 world, const Mesh& mesh, const Material& material)
    {
        bindMaterial(material);
        drawMesh(viewId, world, mesh, material);
    }

    void VulkanRenderDevice::bindMaterial(const Material& material)
    {

    }

    void VulkanRenderDevice::drawMesh(ViewId viewId, const mat4& world, const Mesh& mesh, const Material& material)
    {

    }

    void VulkanRenderDevice::presentFrameBufferToScreen(ViewId viewId, const ScreenFbo& screen)
    {

    }

    void VulkanRenderDevice::presentTextureToScreen(ViewId viewId, const ScreenFbo& screen, const Texture& texture)
    {

    }

    void VulkanRenderDevice::drawMeshInstanced(ViewId viewId, const Mesh& mesh, const Material& material, const void* data, u32 instanceCount, u32 instanceStrideBytes)
    {

    }

    void VulkanRenderDevice::bindFrame(ViewId viewId, const FrameData& u)
    {

    }

    void VulkanRenderDevice::execute(ViewId viewId, const ComputePass& pass)
    {

    }

    void VulkanRenderDevice::requestReadback(const ComputeReadbackRequest& request)
    {

    }

    bool VulkanRenderDevice::pollReadback(ComputeReadbackResult& result)
    {

    }

    void VulkanRenderDevice::drawMeshInstancedGpu(ViewId viewId, const GpuInstanceDrawDesc& desc)
    {

    }

    void VulkanRenderDevice::drawBillboards(ViewId viewId, const GpuBillboardDrawDesc& desc)
    {

    }

    void VulkanRenderDevice::drawTransient(ViewId viewId, const TransientDrawDesc& desc)
    {

    }

}
