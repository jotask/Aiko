#include "vulkan_gpu_profiler.h"

#include <array>
#include <vector>

#include <aiko_macros.h>
#include <intrumentor/profiler.h>

namespace aiko::renderer::vulkan
{
#ifdef AIKO_PROFILER

    void VulkanGpuProfiler::create(VkPhysicalDevice physicalDevice, VkDevice device, uint32_t graphicsQueueFamily, uint32_t frameCount)
    {
        m_device = device;

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);
        m_timestampPeriod = properties.limits.timestampPeriod;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        AIKO_ASSERT(graphicsQueueFamily < queueFamilies.size(), "Invalid graphics queue family for Vulkan GPU profiler");

        m_timestampValidBits = queueFamilies[graphicsQueueFamily].timestampValidBits;

        if (m_timestampValidBits == 0)
        {
            return;
        }

        AIKO_ASSERT(m_timestampValidBits <= 64, "Invalid Vulkan timestamp valid bit count");
        AIKO_ASSERT(frameCount > 0, "Vulkan GPU profiler requires at least one frame");

        const VkQueryPoolCreateInfo queryPoolInfo =
        {
            .sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO,
            .queryType = VK_QUERY_TYPE_TIMESTAMP,
            .queryCount = frameCount * TimestampCountPerFrame,
        };

        const VkResult result = vkCreateQueryPool(m_device, &queryPoolInfo, nullptr, &m_queryPool);

        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan GPU timestamp query pool");

        if (result != VK_SUCCESS)
        {
            m_queryPool = VK_NULL_HANDLE;
            return;
        }

        m_writtenFrames.assign(frameCount, false);
        m_graphicsPassCounts.assign(frameCount, 0);
        m_lastGraphicsGpuMs = 0.0;

    }

    void VulkanGpuProfiler::destroy()
    {
        if (m_queryPool != VK_NULL_HANDLE)
        {
            vkDestroyQueryPool( m_device, m_queryPool, nullptr);
            m_queryPool = VK_NULL_HANDLE;
        }

        m_writtenFrames.clear();
        m_graphicsPassCounts.clear();

        m_timestampPeriod = 0.0f;
        m_timestampValidBits = 0;
        m_lastGraphicsGpuMs = 0.0;
        m_device = VK_NULL_HANDLE;
    }

    void VulkanGpuProfiler::beginGraphicsFrame(VkCommandBuffer commandBuffer,uint32_t frame)
    {
        if (m_queryPool == VK_NULL_HANDLE)
        {
            return;
        }
        AIKO_ASSERT(frame < m_writtenFrames.size(),"Invalid Vulkan GPU profiler frame index");
        const uint32_t firstQuery = frame * TimestampCountPerFrame;
        vkCmdResetQueryPool(commandBuffer, m_queryPool, firstQuery, TimestampCountPerFrame);
        m_graphicsPassCounts[frame] = 0;
        vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, m_queryPool, firstQuery);
        m_writtenFrames[frame] = true;
    }

    void VulkanGpuProfiler::endGraphicsFrame( VkCommandBuffer commandBuffer, uint32_t frame)
    {
        if (m_queryPool == VK_NULL_HANDLE)
        {
            return;
        }

        AIKO_ASSERT(frame < m_writtenFrames.size(), "Invalid Vulkan GPU profiler frame index");

        const uint32_t endQuery = frame * TimestampCountPerFrame + 1;

        vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, m_queryPool, endQuery);
    }

    void VulkanGpuProfiler::resolveGraphicsFrame(uint32_t frame)
    {
        if (m_queryPool == VK_NULL_HANDLE)
        {
            return;
        }

        AIKO_ASSERT(frame < m_writtenFrames.size(), "Invalid Vulkan GPU profiler frame index");

        if (m_writtenFrames[frame] == false)
        {
            return;
        }

        const uint32_t firstQuery = frame * TimestampCountPerFrame;

        const uint32_t passCount = m_graphicsPassCounts[frame];

        AIKO_ASSERT(passCount <= MaxGraphicsPasses, "Invalid Vulkan GPU profiler graphics pass count");

        const uint32_t queryCount = 2 + passCount * 2;

        std::array<uint64_t, TimestampCountPerFrame> timestamps{};

        const VkResult result = vkGetQueryPoolResults(m_device, m_queryPool, firstQuery, queryCount, sizeof(uint64_t) * queryCount, timestamps.data(), sizeof(uint64_t), VK_QUERY_RESULT_64_BIT);

        if (result == VK_NOT_READY)
        {
            return;
        }

        AIKO_ASSERT(result == VK_SUCCESS, "Failed to resolve Vulkan GPU timestamps");

        if (result != VK_SUCCESS)
        {
            return;
        }

        m_lastGraphicsGpuMs = durationMs(timestamps[0], timestamps[1]);

        AIKO_PLOT("Vulkan Graphics GPU ms", m_lastGraphicsGpuMs);

        static constexpr const char* GraphicsPassPlotNames[MaxGraphicsPasses] =
        {
            "Vulkan Graphics Pass 0 GPU ms",
            "Vulkan Graphics Pass 1 GPU ms",
            "Vulkan Graphics Pass 2 GPU ms",
            "Vulkan Graphics Pass 3 GPU ms",
            "Vulkan Graphics Pass 4 GPU ms",
            "Vulkan Graphics Pass 5 GPU ms",
            "Vulkan Graphics Pass 6 GPU ms",
            "Vulkan Graphics Pass 7 GPU ms",
        };

        for (uint32_t pass = 0; pass < passCount; ++pass)
        {
            const uint32_t startTimestamp = 2 + pass * 2;
            const uint32_t endTimestamp = startTimestamp + 1;
            const double passGpuMs = durationMs(timestamps[startTimestamp], timestamps[endTimestamp]);
            AIKO_PLOT(GraphicsPassPlotNames[pass], passGpuMs);
        }
    }

    void VulkanGpuProfiler::beginGraphicsPass(VkCommandBuffer commandBuffer, uint32_t frame)
    {
        if (m_queryPool == VK_NULL_HANDLE)
        {
            return;
        }
        AIKO_ASSERT(frame < m_graphicsPassCounts.size(), "Invalid Vulkan GPU profiler frame index");
        const uint32_t pass = m_graphicsPassCounts[frame];
        AIKO_ASSERT(pass < MaxGraphicsPasses, "Vulkan GPU profiler graphics pass capacity exceeded");
        if (pass >= MaxGraphicsPasses)
        {
            return;
        }
        const uint32_t query = frame * TimestampCountPerFrame + 2 + pass * 2;
        vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, m_queryPool, query);
    }

    void VulkanGpuProfiler::endGraphicsPass(VkCommandBuffer commandBuffer, uint32_t frame)
    {
        if (m_queryPool == VK_NULL_HANDLE)
        {
            return;
        }
        AIKO_ASSERT(frame < m_graphicsPassCounts.size(), "Invalid Vulkan GPU profiler frame index");
        const uint32_t pass = m_graphicsPassCounts[frame];
        AIKO_ASSERT(pass < MaxGraphicsPasses, "Vulkan GPU profiler graphics pass capacity exceeded");
        if (pass >= MaxGraphicsPasses)
        {
            return;
        }
        const uint32_t query = frame * TimestampCountPerFrame + 2 + pass * 2 +1;
        vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, m_queryPool, query);
        ++m_graphicsPassCounts[frame];
    }

    double VulkanGpuProfiler::durationMs(uint64_t startTimestamp, uint64_t endTimestamp) const
    {
        const uint64_t timestampMask = m_timestampValidBits == 64 ? ~uint64_t{0} : (uint64_t{1} << m_timestampValidBits) - 1;
        const uint64_t start = startTimestamp & timestampMask;
        const uint64_t end = endTimestamp & timestampMask;
        const uint64_t ticks = (end - start) & timestampMask;
        return  static_cast<double>(ticks) * static_cast<double>(m_timestampPeriod) / 1'000'000.0;
    }

#endif
}
