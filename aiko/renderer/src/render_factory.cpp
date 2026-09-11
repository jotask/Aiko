#include "render_factory.h"

#include "resources/render_resource_manager.h"

#if defined (AIKO_BGFX)
#include "platform/bgfx/bgfx_render_device.h"
#include "platform/bgfx/impl/bgfx_shader_impl.h"
#include "platform/bgfx/impl/bgfx_texture_impl.h"
#include "platform/bgfx/impl/bgfx_computeshader_impl.h"
#include "platform/bgfx/impl/bgfx_computebuffer_impl.h"
#include "platform/bgfx/impl/bgfx_mesh_impl.h"
#include "platform/bgfx/impl/bgfx_framebuffer_impl.h"
#elif defined (AIKO_NATIVE)


#elif defined (AIKO_VULKAN)
#include "platform/vulkan/vulkan_render_device.h"
#include "platform/vulkan/impl/vulkan_shader_impl.h"
#include "platform/vulkan/impl/vulkan_texture_impl.h"
#include "platform/vulkan/impl/vulkan_computeshader_impl.h"
#include "platform/vulkan/impl/vulkan_computebuffer_impl.h"
#include "platform/vulkan/impl/vulkan_mesh_impl.h"
#include "platform/vulkan/impl/vulkan_framebuffer_impl.h"
#else
#error Backend not supported
#endif

namespace aiko
{
    namespace renderer
    {
        AikoPtr<IRenderDevice> RendererFactory::createRenderDevice(RenderResourceManager* resources)
        {
            #if defined (AIKO_BGFX)
            return std::make_shared<bgfx::BgfxRenderDevice>(resources);
            #elif defined (AIKO_NATIVE)
            #error Not implemented
            #elif defined (AIKO_VULKAN)
            return std::make_shared<vulkan::VulkanRenderDevice>(resources);
            #else
            #error Backend not supported
            #endif
        }

        AikoPtr<interfaces::IShaderImpl> RendererFactory::createShaderImpl()
        {
            #if defined (AIKO_BGFX)
            return std::make_shared<bgfx::BgfxShaderImpl>();
            #elif defined (AIKO_NATIVE)
            #error Not implemented
            #elif defined (AIKO_VULKAN)
            return std::make_shared<vulkan::VulkanShaderImpl>();
            #else
            #error Backend not supported
            #endif
        }

        AikoPtr<interfaces::ITextureImpl> RendererFactory::createTextureImpl()
        {
            #if defined (AIKO_BGFX)
            return std::make_shared<bgfx::BgfxTextureImpl>();
            #elif defined (AIKO_NATIVE)
            #error Not implemented
            #elif defined (AIKO_VULKAN)
            return std::make_shared<vulkan::VulkanTextureImpl>();
            #else
            #error Backend not supported
            #endif
        }

        AikoPtr<interfaces::IFrameBufferImpl> RendererFactory::createFrameBufferImpl()
        {
            #if defined (AIKO_BGFX)
            return std::make_shared<bgfx::BgfxFrameBufferImpl>();
            #elif defined (AIKO_NATIVE)
            #error Not implemented
            #elif defined (AIKO_VULKAN)
            return std::make_shared<vulkan::VulkanFrameBufferImpl>();
            #else
            #error Backend not supported
            #endif
        }

        AikoPtr<interfaces::IComputeShaderImpl> RendererFactory::createComputeShaderImpl()
        {
            #if defined (AIKO_BGFX)
            return std::make_shared<bgfx::BgfxComputeShaderImpl>();
            #elif defined (AIKO_NATIVE)
            #error Not implemented
            #elif defined (AIKO_VULKAN)
            return std::make_shared<vulkan::VulkanComputeShaderImpl>();
            #else
            #error Backend not supported
            #endif
        }

        AikoPtr<interfaces::IComputeBufferImpl> RendererFactory::createComputeBufferImpl()
        {
            #if defined (AIKO_BGFX)
            return std::make_shared<bgfx::BgfxComputeBufferImpl>();
            #elif defined (AIKO_NATIVE)
            #error Not implemented
            #elif defined (AIKO_VULKAN)
            return std::make_shared<vulkan::VulkanComputeBufferImpl>();
            #else
            #error Backend not supported
            #endif
        }

        AikoPtr<interfaces::IMeshImpl> RendererFactory::createMeshImpl(Mesh* mesh)
        {
            #if defined (AIKO_BGFX)
            return std::make_shared<bgfx::BgfxMeshImpl>(mesh);
            #elif defined (AIKO_NATIVE)
            #error Not implemented
            #elif defined (AIKO_VULKAN)
            return std::make_shared<vulkan::VulkanMeshImpl>(mesh);
            #else
            #error Backend not supported
            #endif
        }
    };
}