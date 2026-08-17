#include "vulkan_computeshader_impl.h"

#include <cstring>

#include <logger/logger.h>
#include <core/file.h>

#include "platform/vulkan/vulkan_platform_helper.h"
#include "platform/vulkan/vulkan_types.h"

namespace aiko::renderer::vulkan
{
    VulkanComputeShaderImpl::VulkanComputeShaderImpl()
    {
    }

    bool VulkanComputeShaderImpl::isValid() const
    {
        return false;
    }

    void VulkanComputeShaderImpl::load(string file)
    {
        m_file = file;
    }

    void VulkanComputeShaderImpl::unload()
    {

    }

    uint VulkanComputeShaderImpl::id()
    {
        return {};
    }


}
