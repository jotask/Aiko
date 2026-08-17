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

        virtual void destroy() override;

    private:
        void buildLayout(ComputeBufferFormat format);
    };
}
