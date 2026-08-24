#pragma once

#include "interfaces/i_computebuffer.h"
#include "platform/vulkan/vulkan_types.h"

namespace aiko::renderer::vulkan
{

    class VulkanComputeBufferImpl : public interfaces::IComputeBufferImpl
    {
    public:

        VulkanComputeBufferImpl();
        virtual ~VulkanComputeBufferImpl() override;

        VulkanComputeBufferImpl(const VulkanComputeBufferImpl&) = delete;
        VulkanComputeBufferImpl& operator=(const VulkanComputeBufferImpl&) = delete;

        VulkanComputeBufferImpl(VulkanComputeBufferImpl&&) = delete;
        VulkanComputeBufferImpl& operator=(VulkanComputeBufferImpl&&) = delete;

        virtual bool isValid() const override;

        virtual void create(const ComputeBufferDesc& desc, const void* initialData) override;
        virtual void update(uint32_t start, uint32_t count, const void* data) override;

        virtual void destroy() override;

        VkBuffer buffer() const { return m_buffer; }
        VkDeviceSize size() const { return m_elementSize * static_cast<VkDeviceSize>(m_count); }
        uint32_t count() const { return m_count; }
        VkDeviceSize elementSize() const { return m_elementSize; }

        const VulkanBufferState& state() const { return m_state; }
        void setState(const VulkanBufferState& state) { m_state = state; }

        ComputeBufferUsage usage() const { return m_usage; }

        bool hasPendingUploads() const { return m_pendingUploads.empty() == false; }

        struct PendingUpload
        {
            VkDeviceSize offset = 0;
            vector<uint8_t> data;
        };

        std::vector<PendingUpload> takePendingUploads();

    private:

        VkBuffer m_buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_memory = VK_NULL_HANDLE;

        VkDeviceSize m_elementSize = 0;
        uint32_t m_count = 0;

        ComputeBufferUsage m_usage = ComputeBufferUsage::None;

        VulkanBufferState m_state{};

        vector<PendingUpload> m_pendingUploads;

        void buildLayout(ComputeBufferFormat format);
        VkBufferUsageFlags buildUsageFlags(ComputeBufferUsage usage) const;

    };
}
