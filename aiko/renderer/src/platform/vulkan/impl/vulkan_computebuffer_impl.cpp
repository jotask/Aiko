#include "vulkan_computebuffer_impl.h"

#include "platform/vulkan/vulkan_context.h"

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
        AIKO_ASSERT(m_buffer == VK_NULL_HANDLE && m_memory == VK_NULL_HANDLE, "VulkanComputeBufferImpl destroyed without destroy()");
    }

    bool VulkanComputeBufferImpl::isValid() const
    {
        return m_buffer != VK_NULL_HANDLE && m_memory != VK_NULL_HANDLE && m_elementSize > 0 && m_count > 0;
    }

    void VulkanComputeBufferImpl::create(ComputeBufferFormat format, uint32_t count, const void* initialData, ComputeAccess access)
    {
        destroy();

        AIKO_ASSERT(count > 0, "Invalid compute buffer count");

        buildLayout(format);

        m_count = count;
        m_access = access;

        const VkDeviceSize size = m_elementSize * static_cast<VkDeviceSize>(m_count);

        VulkanContext& ctx = VulkanContext::current();

        ctx.createBuffer(size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_buffer, m_memory);

        if (initialData != nullptr)
        {
            void* mapped = nullptr;

            const VkResult result = vkMapMemory(ctx.device(), m_memory, 0, size, 0, &mapped);
            AIKO_ASSERT( result == VK_SUCCESS,"Failed to map Vulkan compute buffer");

            std::memcpy(mapped, initialData, static_cast<size_t>(size));
            vkUnmapMemory(ctx.device(), m_memory);
        }
    }

    void VulkanComputeBufferImpl::update(uint32_t start, uint32_t count, const void* data)
    {
        AIKO_ASSERT(isValid(), "Invalid compute buffer");
        AIKO_ASSERT(data != nullptr, "Compute buffer update data is null");
        AIKO_ASSERT(start + count <= m_count, "Compute buffer update out of range");

        if (count == 0)
        {
            return;
        }

        VulkanContext& ctx = VulkanContext::current();

        const VkDeviceSize offset =
            static_cast<VkDeviceSize>(start) * m_elementSize;

        const VkDeviceSize size =
            static_cast<VkDeviceSize>(count) * m_elementSize;

        void* mapped = nullptr;

        const VkResult result = vkMapMemory(
            ctx.device(),
            m_memory,
            offset,
            size,
            0,
            &mapped
        );

        AIKO_ASSERT(
            result == VK_SUCCESS,
            "Failed to map Vulkan compute buffer"
        );

        std::memcpy(
            mapped,
            data,
            static_cast<size_t>(size)
        );

        vkUnmapMemory(
            ctx.device(),
            m_memory
        );
    }

    void VulkanComputeBufferImpl::readback(uint32_t start, uint32_t count, void* dst) const
    {
        AIKO_ASSERT(isValid(), "Invalid compute buffer");
        AIKO_ASSERT(dst != nullptr, "Compute buffer readback destination is null");
        AIKO_ASSERT(start + count <= m_count, "Compute buffer readback out of range");

        if (count == 0)
        {
            return;
        }

        VulkanContext& ctx = VulkanContext::current();

        const VkDeviceSize offset = static_cast<VkDeviceSize>(start) * m_elementSize;
        const VkDeviceSize size = static_cast<VkDeviceSize>(count) * m_elementSize;

        void* mapped = nullptr;

        const VkResult result = vkMapMemory(ctx.device(), m_memory, offset, size, 0, &mapped);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to map compute buffer for readback");

        std::memcpy(dst, mapped, static_cast<size_t>(size));

        vkUnmapMemory(ctx.device(), m_memory);
    }

    void VulkanComputeBufferImpl::destroy()
    {
        if (m_buffer == VK_NULL_HANDLE &&
                m_memory == VK_NULL_HANDLE)
        {
            m_elementSize = 0;
            m_count = 0;
            return;
        }

        VulkanContext& ctx = VulkanContext::current();
        VkDevice device = ctx.device();

        if (m_buffer != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(device, m_buffer, nullptr);
        }

        if (m_memory != VK_NULL_HANDLE)
        {
            vkFreeMemory(device, m_memory, nullptr);
        }

        m_buffer = VK_NULL_HANDLE;
        m_memory = VK_NULL_HANDLE;

        m_elementSize = 0;
        m_count = 0;
    }

    void VulkanComputeBufferImpl::buildLayout(ComputeBufferFormat format)
    {
        switch (format)
        {
            case ComputeBufferFormat::Vec4f:
                m_elementSize = sizeof(vec4);
                break;

            default:
                AIKO_ASSERT(false, "Unsupported Vulkan compute buffer format");
                break;
        }
    }

}
