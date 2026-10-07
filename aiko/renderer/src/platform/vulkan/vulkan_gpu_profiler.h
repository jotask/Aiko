#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <deque>

#include <volk.h>

namespace aiko::renderer::vulkan
{
    class VulkanGpuProfiler final
    {
    public:
        VulkanGpuProfiler() = default;

        #ifdef AIKO_PROFILER
        void create(VkPhysicalDevice physicalDevice, VkDevice device, uint32_t graphicsQueueFamily, uint32_t computeQueueFamily, uint32_t frameCount);
        void destroy();
        void beginGraphicsFrame(VkCommandBuffer commandBuffer, uint32_t frame);
        void endGraphicsFrame(VkCommandBuffer commandBuffer, uint32_t frame);
        void resolveGraphicsFrame(uint32_t frame);
        void beginGraphicsPass(VkCommandBuffer commandBuffer, uint32_t frame);
        void endGraphicsPass(VkCommandBuffer commandBuffer, uint32_t frame);
        double graphicsGpuMs() const { return m_lastGraphicsGpuMs; }
        void beginComputeFrame(VkCommandBuffer commandBuffer, uint32_t frame);
        void endComputeFrame(VkCommandBuffer commandBuffer, uint32_t frame);
        void resolveComputeFrame(uint32_t frame);
        void beginComputePass(VkCommandBuffer commandBuffer, uint32_t frame, std::string_view name);
        void endComputePass(VkCommandBuffer commandBuffer, uint32_t frame);
        #else
        void create(VkPhysicalDevice, VkDevice, uint32_t, uint32_t, uint32_t) {}
        void destroy() {}
        void beginGraphicsFrame(VkCommandBuffer, uint32_t) {}
        void endGraphicsFrame(VkCommandBuffer, uint32_t) {}
        void resolveGraphicsFrame(uint32_t) {}
        void beginGraphicsPass(VkCommandBuffer, uint32_t) {}
        void endGraphicsPass(VkCommandBuffer, uint32_t) {}
        double graphicsGpuMs() const { return 0.0; }
        void beginComputeFrame(VkCommandBuffer, uint32_t) {}
        void endComputeFrame(VkCommandBuffer, uint32_t) {}
        void resolveComputeFrame(uint32_t) {}
        void beginComputePass(VkCommandBuffer, uint32_t, std::string_view) {}
        void endComputePass(VkCommandBuffer, uint32_t) {}
        #endif

    private:
        #ifdef AIKO_PROFILER
        static constexpr uint32_t MaxGraphicsPasses = 8;
        static constexpr uint32_t GraphicsTimestampCountPerFrame = 2 + MaxGraphicsPasses * 2;

        static constexpr uint32_t MaxComputePasses = 256;
        static constexpr uint32_t ComputeTimestampCountPerFrame = 2 + MaxComputePasses * 2;

        VkDevice m_device = VK_NULL_HANDLE;

        VkQueryPool m_graphicsQueryPool = VK_NULL_HANDLE;
        VkQueryPool m_computeQueryPool = VK_NULL_HANDLE;

        float m_timestampPeriod = 0.0f;

        uint32_t m_graphicsTimestampValidBits = 0;
        uint32_t m_computeTimestampValidBits = 0;

        std::vector<bool> m_graphicsWrittenFrames;
        std::vector<uint32_t> m_graphicsPassCounts;

        std::vector<bool> m_computeWrittenFrames;
        std::vector<uint32_t> m_computePassCounts;
        std::vector<std::vector<const char*>> m_computePassNames;
        std::deque<std::string> m_computeNameStorage;

        double m_lastGraphicsGpuMs = 0.0;
        double m_lastComputeGpuMs = 0.0;

        double durationMs(uint64_t startTimestamp, uint64_t endTimestamp, uint32_t timestampValidBits) const;
        const char* internComputeName(std::string_view name);
        #endif
    };
}
