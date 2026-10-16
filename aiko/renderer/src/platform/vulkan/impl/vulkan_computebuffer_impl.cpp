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
        AIKO_ASSERT(m_readbackRetainCount == 0, "VulkanComputeBufferImpl destroyed with retained readback");
        AIKO_ASSERT(m_buffer == VK_NULL_HANDLE && m_memory == VK_NULL_HANDLE, "VulkanComputeBufferImpl destroyed without destroy()");
    }

    bool VulkanComputeBufferImpl::isValid() const
    {
        return m_destroyPending == false && m_buffer != VK_NULL_HANDLE && m_memory != VK_NULL_HANDLE && m_elementSize > 0 && m_count > 0;
    }

    void VulkanComputeBufferImpl::create(const ComputeBufferDesc& desc, const void* initialData)
    {
        AIKO_ASSERT(m_readbackRetainCount == 0, "Cannot recreate compute buffer while readback is pending");
        AIKO_ASSERT(desc.count > 0, "Invalid compute buffer count");

        const VkDeviceSize elementSize = buildLayout(desc.format, desc.stride);
        const VkDeviceSize size = elementSize * static_cast<VkDeviceSize>(desc.count);

        const VkBufferUsageFlags usageFlags = buildUsageFlags(desc.usage);

        // Prepare initial uploads without changing the current buffer.
        std::vector<PendingUpload> pendingUploads;

        if (initialData != nullptr)
        {
            AIKO_ASSERT(hasFlag(desc.usage, ComputeBufferUsage::TransferDst), "Compute buffer initial data requires transfer-destination usage");

            PendingUpload upload{};
            upload.offset = 0;
            upload.data.resize(static_cast<size_t>(size));

            std::memcpy(upload.data.data(), initialData, static_cast<size_t>(size));

            pendingUploads.push_back(std::move(upload));
        }

        VulkanContext& ctx = VulkanContext::current();

        VkBuffer newBuffer = VK_NULL_HANDLE;
        VkDeviceMemory newMemory = VK_NULL_HANDLE;

        const VkResult result = ctx.createBuffer(size, usageFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, newBuffer, newMemory);

        if (result != VK_SUCCESS)
        {
            logger::Log::error("Failed to create Vulkan compute buffer (VkResult %d)", static_cast<int>(result));
            return;
        }

        // Allocation succeeded: retire the previous buffer.
        destroyNow();

        // Commit the complete replacement state.
        m_buffer = newBuffer;
        m_memory = newMemory;

        m_elementSize = elementSize;
        m_count = desc.count;
        m_format = desc.format;
        m_usage = desc.usage;

        m_state =
        {
            .stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            .access = 0,
        };

        m_pendingUploads = std::move(pendingUploads);
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
        if (m_readbackRetainCount > 0)
        {
            m_destroyPending = true;
            return;
        }
        destroyNow();
    }

    std::vector<VulkanComputeBufferImpl::PendingUpload> VulkanComputeBufferImpl::takePendingUploads()
    {
        std::vector<PendingUpload> uploads = std::move(m_pendingUploads);
        m_pendingUploads.clear();
        return uploads;
    }

    void VulkanComputeBufferImpl::retainReadback()
    {
        AIKO_ASSERT(m_destroyPending == false, "Cannot retain compute buffer pending destruction");
        AIKO_ASSERT(m_buffer != VK_NULL_HANDLE && m_memory != VK_NULL_HANDLE, "Cannot retain invalid compute buffer");
        ++m_readbackRetainCount;
    }

    void VulkanComputeBufferImpl::releaseReadback()
    {
        AIKO_ASSERT(m_readbackRetainCount > 0, "Compute buffer readback retain underflow");
        --m_readbackRetainCount;
        if (m_readbackRetainCount == 0 && m_destroyPending)
        {
            destroyNow();
        }
    }

    void VulkanComputeBufferImpl::destroyNow()
    {

        AIKO_ASSERT(m_readbackRetainCount == 0, "Cannot destroy compute buffer while readback is retained");

        if (m_buffer == VK_NULL_HANDLE && m_memory == VK_NULL_HANDLE)
        {
            m_elementSize = 0;
            m_count = 0;
            m_format = ComputeBufferFormat::Vec4f;
            m_usage = ComputeBufferUsage::None;
            m_state = {};
            m_pendingUploads.clear();
            m_destroyPending = false;
            return;
        }

        const VkBuffer buffer = m_buffer;
        const VkDeviceMemory memory = m_memory;

        m_buffer = VK_NULL_HANDLE;
        m_memory = VK_NULL_HANDLE;

        m_elementSize = 0;
        m_count = 0;
        m_format = ComputeBufferFormat::Vec4f;
        m_usage = ComputeBufferUsage::None;
        m_state = {};
        m_pendingUploads.clear();
        m_destroyPending = false;

        VulkanContext::current().retireBuffer(buffer, memory);
    }


    VkDeviceSize VulkanComputeBufferImpl::buildLayout(ComputeBufferFormat format, uint32_t stride)
    {
        switch (format)
        {
            case ComputeBufferFormat::Vec4f:
                return sizeof(vec4);

            case ComputeBufferFormat::Uint32:
                return sizeof(uint32_t);

            case ComputeBufferFormat::Structured:
                AIKO_ASSERT(stride > 0, "Structured compute buffer requires a stride");
                AIKO_ASSERT(stride % 4 == 0, "Structured compute buffer stride must be 4-byte aligned");
                return stride;

            default:
                AIKO_ASSERT(false, "Unsupported Vulkan compute buffer format");
                return 0;
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
