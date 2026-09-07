#pragma once

#include <vector>

#include <volk.h>

namespace aiko::renderer::vulkan
{
    class VulkanGpuProfiler final
    {
    public:
        VulkanGpuProfiler() = default;

        #ifdef AIKO_PROFILER
        void create(VkPhysicalDevice physicalDevice, VkDevice device, uint32_t graphicsQueueFamily, uint32_t frameCount);
        void destroy();
        void beginGraphicsFrame(VkCommandBuffer commandBuffer, uint32_t frame);
        void endGraphicsFrame(VkCommandBuffer commandBuffer, uint32_t frame);
        void resolveGraphicsFrame(uint32_t frame);
        void beginGraphicsPass(VkCommandBuffer commandBuffer, uint32_t frame);
        void endGraphicsPass(VkCommandBuffer commandBuffer, uint32_t frame);
        double graphicsGpuMs() const { return m_lastGraphicsGpuMs; }
        #else
        void create(VkPhysicalDevice, VkDevice, uint32_t, uint32_t) {}
        void destroy() {}
        void beginGraphicsFrame(VkCommandBuffer, uint32_t) {}
        void endGraphicsFrame(VkCommandBuffer, uint32_t) {}
        void resolveGraphicsFrame(uint32_t) {}
        void beginGraphicsPass(VkCommandBuffer, uint32_t) {}
        void endGraphicsPass(VkCommandBuffer, uint32_t) {}
        double graphicsGpuMs() const { return 0.0; }
        #endif

    private:
        #ifdef AIKO_PROFILER
        static constexpr uint32_t MaxGraphicsPasses = 8;
        static constexpr uint32_t TimestampCountPerFrame = 2 + MaxGraphicsPasses * 2;

        VkDevice m_device = VK_NULL_HANDLE;
        VkQueryPool m_queryPool = VK_NULL_HANDLE;

        float m_timestampPeriod = 0.0f;
        uint32_t m_timestampValidBits = 0;

        std::vector<bool> m_writtenFrames;
        std::vector<uint32_t> m_graphicsPassCounts;

        double m_lastGraphicsGpuMs = 0.0;

        double durationMs(uint64_t startTimestamp, uint64_t endTimestamp) const;
        #endif
    };
}
