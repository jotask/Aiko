#include "vulkan_computebuffer_impl.h"

#include <cstring>

#include <logger/logger.h>
#include <core/file.h>

#include "platform/vulkan/vulkan_platform_helper.h"
#include "platform/vulkan/vulkan_types.h"

namespace aiko::renderer::vulkan
{
    VulkanComputeBufferImpl::VulkanComputeBufferImpl()
    {
    }

    VulkanComputeBufferImpl::~VulkanComputeBufferImpl()
    {

    }

    bool VulkanComputeBufferImpl::isValid() const
    {
        return false;
    }

    void VulkanComputeBufferImpl::create(ComputeBufferFormat format, uint32_t count, const void* initialData, ComputeAccess access)
    {

    }

    void VulkanComputeBufferImpl::update(uint32_t start, uint32_t count, const void* data)
    {

    }

    void VulkanComputeBufferImpl::destroy()
    {

    }

    void VulkanComputeBufferImpl::buildLayout(ComputeBufferFormat format)
    {

    }

}
