#include "vulkan_framebuffer_impl.h"

#include <random>

#include <core/file.h>

#include "platform/vulkan/vulkan_types.h"
#include "platform/vulkan/impl/vulkan_texture_impl.h"

namespace aiko::renderer::vulkan
{

    VulkanFrameBufferImpl::VulkanFrameBufferImpl()
    {
    }

    uint VulkanFrameBufferImpl::id() const
    {
        return {};
    }

    void VulkanFrameBufferImpl::use()
    {

    }

    void VulkanFrameBufferImpl::unuse()
    {

    }

    bool VulkanFrameBufferImpl::isValid() const
    {
        return false;
    }

    void VulkanFrameBufferImpl::create(Texture& color, Texture& depth)
    {

    }

    void VulkanFrameBufferImpl::unload()
    {

    }

}
