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

    void VulkanComputeBufferImpl::create(const ComputeBufferDesc& desc, const void* initialData)
    {
        destroy();

        AIKO_ASSERT(desc.count > 0, "Invalid compute buffer count");

        buildLayout(desc.format);

        m_count = desc.count;
        m_usage = desc.usage;

        const VkDeviceSize size = m_elementSize * static_cast<VkDeviceSize>(m_count);

        VulkanContext& ctx = VulkanContext::current();

        const VkBufferUsageFlags usageFlags = buildUsageFlags(desc.usage);

        ctx.createBuffer(size, usageFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_buffer, m_memory);

        m_state =
        {
            .stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            .access = 0,
        };

        if (initialData != nullptr)
        {
            AIKO_ASSERT( hasFlag( m_usage, ComputeBufferUsage::TransferDst), "Compute buffer initial data requires transfer-destination usage");

            PendingUpload upload{};
            upload.offset = 0;
            upload.data.resize(static_cast<size_t>(size));

            std::memcpy( upload.data.data(), initialData, static_cast<size_t>(size));
            m_pendingUploads.push_back(std::move(upload));

            m_state =
            {
                .stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                .access = 0,
            };
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

        AIKO_ASSERT(hasFlag(m_usage, ComputeBufferUsage::TransferDst), "Compute buffer update requires transfer-destination usage");

        const VkDeviceSize offset = static_cast<VkDeviceSize>(start) * m_elementSize;

        const VkDeviceSize size = static_cast<VkDeviceSize>(count) * m_elementSize;

        PendingUpload upload{};
        upload.offset = offset;
        upload.data.resize(static_cast<size_t>(size));

        std::memcpy(upload.data.data(), data, static_cast<size_t>(size));

        m_pendingUploads.push_back(std::move(upload));

    }

    void VulkanComputeBufferImpl::destroy()
    {
        if (m_buffer == VK_NULL_HANDLE && m_memory == VK_NULL_HANDLE)
        {
            m_elementSize = 0;
            m_count = 0;
            m_state = {};
            m_pendingUploads.clear();
            return;
        }

        const VkBuffer buffer = m_buffer;
        const VkDeviceMemory memory = m_memory;

        m_buffer = VK_NULL_HANDLE;
        m_memory = VK_NULL_HANDLE;

        m_elementSize = 0;
        m_count = 0;
        m_state = {};
        m_pendingUploads.clear();

        VulkanContext::current().retireBuffer(buffer, memory);
    }

    std::vector<VulkanComputeBufferImpl::PendingUpload> VulkanComputeBufferImpl::takePendingUploads()
    {
        std::vector<PendingUpload> uploads = std::move(m_pendingUploads);
        m_pendingUploads.clear();
        return uploads;
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

    VkBufferUsageFlags VulkanComputeBufferImpl::buildUsageFlags(ComputeBufferUsage usage) const
    {
        VkBufferUsageFlags flags = 0;

        if (hasFlag(usage, ComputeBufferUsage::Storage))
        {
            flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        }

        if (hasFlag(usage, ComputeBufferUsage::TransferSrc))
        {
            flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        }

        if (hasFlag( usage, ComputeBufferUsage::TransferDst))
        {
            flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }

        if (hasFlag( usage, ComputeBufferUsage::Vertex))
        {
            flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        }

        if (hasFlag( usage, ComputeBufferUsage::Index))
        {
            flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }

        if (hasFlag( usage, ComputeBufferUsage::Indirect))
        {
            flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        }

        AIKO_ASSERT(flags != 0, "Compute buffer requires at least one usage");

        return flags;
    }

}
