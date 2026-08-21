#pragma once

#include "interfaces/i_computebuffer.h"

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

        virtual void create(ComputeBufferFormat format, uint32_t count, const void* initialData, ComputeAccess access) override;
        virtual void update(uint32_t start, uint32_t count, const void* data) override;
        void read(void* destination, VkDeviceSize size);

        virtual void destroy() override;

        VkBuffer buffer() const { return m_buffer; }
        VkDeviceSize size() const { return m_elementSize * static_cast<VkDeviceSize>(m_count); }
        uint32_t count() const { return m_count; }
        VkDeviceSize elementSize() const { return m_elementSize; }

    private:
        void buildLayout(ComputeBufferFormat format);

        VkBuffer m_buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_memory = VK_NULL_HANDLE;

        VkDeviceSize m_elementSize = 0;
        uint32_t m_count = 0;

        ComputeAccess m_access = ComputeAccess::ReadWrite;

    };
}
