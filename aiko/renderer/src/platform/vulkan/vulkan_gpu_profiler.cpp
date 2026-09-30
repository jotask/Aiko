#include "vulkan_gpu_profiler.h"

#include <array>
#include <vector>

#include <aiko_macros.h>
#include <intrumentor/profiler.h>

namespace aiko::renderer::vulkan
{
#ifdef AIKO_PROFILER

    void VulkanGpuProfiler::create(VkPhysicalDevice physicalDevice, VkDevice device, uint32_t graphicsQueueFamily, uint32_t computeQueueFamily, uint32_t frameCount)
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
        AIKO_ASSERT(computeQueueFamily < queueFamilies.size(), "Invalid compute queue family for Vulkan GPU profiler");

        m_graphicsTimestampValidBits = queueFamilies[graphicsQueueFamily].timestampValidBits;
        m_computeTimestampValidBits = queueFamilies[computeQueueFamily].timestampValidBits;

        AIKO_ASSERT(m_graphicsTimestampValidBits <= 64, "Invalid Vulkan graphics timestamp valid bit count");
        AIKO_ASSERT(m_computeTimestampValidBits <= 64, "Invalid Vulkan compute timestamp valid bit count");
        AIKO_ASSERT(frameCount > 0, "Vulkan GPU profiler requires at least one frame");

        if (m_graphicsTimestampValidBits > 0)
        {
            const VkQueryPoolCreateInfo graphicsQueryPoolInfo =
            {
                .sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO,
                .queryType = VK_QUERY_TYPE_TIMESTAMP,
                .queryCount = frameCount * GraphicsTimestampCountPerFrame,
            };

            const VkResult result = vkCreateQueryPool(m_device, &graphicsQueryPoolInfo, nullptr, &m_graphicsQueryPool);
            AIKO_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan graphics GPU timestamp query pool");

            if (result != VK_SUCCESS)
            {
                m_graphicsQueryPool = VK_NULL_HANDLE;
            }
        }

        if (m_computeTimestampValidBits > 0)
        {
            const VkQueryPoolCreateInfo computeQueryPoolInfo =
            {
                .sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO,
                .queryType = VK_QUERY_TYPE_TIMESTAMP,
                .queryCount = frameCount * ComputeTimestampCountPerFrame,
            };

            const VkResult result = vkCreateQueryPool(m_device, &computeQueryPoolInfo, nullptr, &m_computeQueryPool);

            AIKO_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan compute GPU timestamp query pool");

            if (result != VK_SUCCESS)
            {
                m_computeQueryPool = VK_NULL_HANDLE;
            }
        }

        m_graphicsWrittenFrames.assign(frameCount, false);
        m_graphicsPassCounts.assign(frameCount, 0);

        m_computeWrittenFrames.assign(frameCount, false);
        m_computePassCounts.assign(frameCount, 0);
        m_computePassNames.resize(frameCount);

        m_lastGraphicsGpuMs = 0.0;
        m_lastComputeGpuMs = 0.0;

    }

    void VulkanGpuProfiler::destroy()
    {
        if (m_graphicsQueryPool != VK_NULL_HANDLE)
        {
            vkDestroyQueryPool(m_device, m_graphicsQueryPool, nullptr);
            m_graphicsQueryPool = VK_NULL_HANDLE;
        }

        if (m_computeQueryPool != VK_NULL_HANDLE)
        {
            vkDestroyQueryPool(m_device, m_computeQueryPool, nullptr);
            m_computeQueryPool = VK_NULL_HANDLE;
        }

        m_graphicsWrittenFrames.clear();
        m_graphicsPassCounts.clear();

        m_computeWrittenFrames.clear();
        m_computePassCounts.clear();
        m_computePassNames.clear();
        m_computeNameStorage.clear();

        m_timestampPeriod = 0.0f;

        m_graphicsTimestampValidBits = 0;
        m_computeTimestampValidBits = 0;

        m_lastGraphicsGpuMs = 0.0;
        m_lastComputeGpuMs = 0.0;

        m_device = VK_NULL_HANDLE;
    }

    void VulkanGpuProfiler::beginGraphicsFrame(VkCommandBuffer commandBuffer,uint32_t frame)
    {
        if (m_graphicsQueryPool == VK_NULL_HANDLE)
        {
            return;
        }
        AIKO_ASSERT(frame < m_graphicsWrittenFrames.size(),"Invalid Vulkan GPU profiler frame index");
        const uint32_t firstQuery = frame * GraphicsTimestampCountPerFrame;
        vkCmdResetQueryPool(commandBuffer, m_graphicsQueryPool, firstQuery, GraphicsTimestampCountPerFrame);
        m_graphicsPassCounts[frame] = 0;
        vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, m_graphicsQueryPool, firstQuery);
        m_graphicsWrittenFrames[frame] = true;
    }

    void VulkanGpuProfiler::endGraphicsFrame( VkCommandBuffer commandBuffer, uint32_t frame)
    {
        if (m_graphicsQueryPool == VK_NULL_HANDLE)
        {
            return;
        }

        AIKO_ASSERT(frame < m_graphicsWrittenFrames.size(), "Invalid Vulkan GPU profiler frame index");

        const uint32_t endQuery = frame * GraphicsTimestampCountPerFrame + 1;

        vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, m_graphicsQueryPool, endQuery);
    }

    void VulkanGpuProfiler::resolveGraphicsFrame(uint32_t frame)
    {
        if (m_graphicsQueryPool == VK_NULL_HANDLE)
        {
            return;
        }

        AIKO_ASSERT(frame < m_graphicsWrittenFrames.size(), "Invalid Vulkan GPU profiler frame index");

        if (m_graphicsWrittenFrames[frame] == false)
        {
            return;
        }

        const uint32_t firstQuery = frame * GraphicsTimestampCountPerFrame;

        const uint32_t passCount = m_graphicsPassCounts[frame];

        AIKO_ASSERT(passCount <= MaxGraphicsPasses, "Invalid Vulkan GPU profiler graphics pass count");

        const uint32_t queryCount = 2 + passCount * 2;

        std::array<uint64_t, GraphicsTimestampCountPerFrame> timestamps{};

        const VkResult result = vkGetQueryPoolResults(m_device, m_graphicsQueryPool, firstQuery, queryCount, sizeof(uint64_t) * queryCount, timestamps.data(), sizeof(uint64_t), VK_QUERY_RESULT_64_BIT);

        if (result == VK_NOT_READY)
        {
            return;
        }

        AIKO_ASSERT(result == VK_SUCCESS, "Failed to resolve Vulkan GPU timestamps");

        if (result != VK_SUCCESS)
        {
            return;
        }

        m_lastGraphicsGpuMs = durationMs(timestamps[0], timestamps[1], m_graphicsTimestampValidBits);

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
            const double passGpuMs = durationMs(timestamps[startTimestamp], timestamps[endTimestamp], m_graphicsTimestampValidBits);
            AIKO_PLOT(GraphicsPassPlotNames[pass], passGpuMs);
        }
    }

    void VulkanGpuProfiler::beginGraphicsPass(VkCommandBuffer commandBuffer, uint32_t frame)
    {
        if (m_graphicsQueryPool == VK_NULL_HANDLE)
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
        const uint32_t query = frame * GraphicsTimestampCountPerFrame + 2 + pass * 2;
        vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, m_graphicsQueryPool, query);
    }

    void VulkanGpuProfiler::endGraphicsPass(VkCommandBuffer commandBuffer, uint32_t frame)
    {
        if (m_graphicsQueryPool == VK_NULL_HANDLE)
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
        const uint32_t query = frame * GraphicsTimestampCountPerFrame + 2 + pass * 2 +1;
        vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, m_graphicsQueryPool, query);
        ++m_graphicsPassCounts[frame];
    }

    void VulkanGpuProfiler::beginComputeFrame(VkCommandBuffer commandBuffer, uint32_t frame)
    {
        if (m_computeQueryPool == VK_NULL_HANDLE)
        {
            return;
        }

        AIKO_ASSERT(frame < m_computeWrittenFrames.size(), "Invalid Vulkan GPU profiler compute frame index");

        const uint32_t firstQuery = frame * ComputeTimestampCountPerFrame;

        vkCmdResetQueryPool(commandBuffer, m_computeQueryPool, firstQuery, ComputeTimestampCountPerFrame);

        m_computePassCounts[frame] = 0;
        m_computePassNames[frame].clear();

        vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, m_computeQueryPool, firstQuery);

        m_computeWrittenFrames[frame] = true;
    }

    void VulkanGpuProfiler::endComputeFrame(VkCommandBuffer commandBuffer, uint32_t frame)
    {
        if (m_computeQueryPool == VK_NULL_HANDLE)
        {
            return;
        }

        AIKO_ASSERT(frame < m_computeWrittenFrames.size(), "Invalid Vulkan GPU profiler compute frame index");

        const uint32_t endQuery = frame * ComputeTimestampCountPerFrame + 1;

        vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, m_computeQueryPool, endQuery);
    }

    void VulkanGpuProfiler::beginComputePass(VkCommandBuffer commandBuffer, uint32_t frame, std::string_view name)
    {
        if (m_computeQueryPool == VK_NULL_HANDLE)
        {
            return;
        }

        AIKO_ASSERT(frame < m_computePassCounts.size(), "Invalid Vulkan GPU profiler compute frame index");

        const uint32_t pass = m_computePassCounts[frame];

        AIKO_ASSERT(pass < MaxComputePasses, "Vulkan GPU profiler compute pass capacity exceeded");

        if (pass >= MaxComputePasses)
        {
            return;
        }

        const uint32_t query = frame * ComputeTimestampCountPerFrame + 2 + pass * 2;

        vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, m_computeQueryPool, query);

        m_computePassNames[frame].push_back(internComputeName(name));
    }

    void VulkanGpuProfiler::endComputePass(VkCommandBuffer commandBuffer, uint32_t frame)
    {
        if (m_computeQueryPool == VK_NULL_HANDLE)
        {
            return;
        }

        AIKO_ASSERT(frame < m_computePassCounts.size(), "Invalid Vulkan GPU profiler compute frame index");

        const uint32_t pass = m_computePassCounts[frame];

        AIKO_ASSERT(pass < MaxComputePasses, "Vulkan GPU profiler compute pass capacity exceeded");

        if (pass >= MaxComputePasses)
        {
            return;
        }

        const uint32_t query = frame * ComputeTimestampCountPerFrame + 2 + pass * 2 + 1;

        vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, m_computeQueryPool, query);

        ++m_computePassCounts[frame];
    }

    void VulkanGpuProfiler::resolveComputeFrame(uint32_t frame)
    {
        if (m_computeQueryPool == VK_NULL_HANDLE)
        {
            return;
        }

        AIKO_ASSERT(frame < m_computeWrittenFrames.size(), "Invalid Vulkan GPU profiler compute frame index");

        if (m_computeWrittenFrames[frame] == false)
        {
            return;
        }

        const uint32_t firstQuery = frame * ComputeTimestampCountPerFrame;

        const uint32_t passCount = m_computePassCounts[frame];

        AIKO_ASSERT(passCount <= MaxComputePasses, "Invalid Vulkan GPU profiler compute pass count");

        const uint32_t queryCount = 2 + passCount * 2;

        std::array<uint64_t, ComputeTimestampCountPerFrame> timestamps{};

        const VkResult result = vkGetQueryPoolResults(m_device, m_computeQueryPool, firstQuery, queryCount, sizeof(uint64_t) * queryCount, timestamps.data(), sizeof(uint64_t), VK_QUERY_RESULT_64_BIT);

        if (result == VK_NOT_READY)
        {
            return;
        }

        AIKO_ASSERT(result == VK_SUCCESS, "Failed to resolve Vulkan compute GPU timestamps");

        if (result != VK_SUCCESS)
        {
            return;
        }

        m_lastComputeGpuMs = durationMs(timestamps[0], timestamps[1], m_computeTimestampValidBits);

        AIKO_PLOT("Vulkan Compute GPU ms", m_lastComputeGpuMs);
        AIKO_ASSERT(m_computePassNames[frame].size() == passCount, "Vulkan GPU profiler compute pass name count mismatch");

        std::vector<const char*> timingNames;
        std::vector<double> timingValues;

        timingNames.reserve(passCount);
        timingValues.reserve(passCount);

        for (uint32_t pass = 0; pass < passCount; ++pass)
        {
            const uint32_t startTimestamp = 2 + pass * 2;
            const uint32_t endTimestamp = startTimestamp + 1;

            const double passGpuMs = durationMs(timestamps[startTimestamp], timestamps[endTimestamp], m_computeTimestampValidBits);

            const char* name = m_computePassNames[frame][pass];

            bool found = false;

            for (uint32_t timing = 0; timing < timingNames.size(); ++timing)
            {
                if (timingNames[timing] == name)
                {
                    timingValues[timing] += passGpuMs;
                    found = true;
                    break;
                }
            }

            if (found == false)
            {
                timingNames.push_back(name);
                timingValues.push_back(passGpuMs);
            }
        }

        for (uint32_t timing = 0; timing < timingNames.size(); ++timing)
        {
            AIKO_PLOT(timingNames[timing], timingValues[timing]);
        }
    }

    double VulkanGpuProfiler::durationMs(uint64_t startTimestamp, uint64_t endTimestamp, uint32_t timestampValidBits) const
    {
        AIKO_ASSERT(timestampValidBits > 0 && timestampValidBits <= 64, "Invalid Vulkan timestamp valid bit count");

        const uint64_t timestampMask = timestampValidBits == 64 ? ~uint64_t{0} : (uint64_t{1} << timestampValidBits) - 1;

        const uint64_t start = startTimestamp & timestampMask;
        const uint64_t end = endTimestamp & timestampMask;

        const uint64_t ticks = (end - start) & timestampMask;

        return static_cast<double>(ticks) * static_cast<double>(m_timestampPeriod) / 1'000'000.0;
    }

    const char* VulkanGpuProfiler::internComputeName(std::string_view name)
    {
        for (const std::string& existing : m_computeNameStorage)
        {
            if (existing == name)
            {
                return existing.c_str();
            }
        }

        m_computeNameStorage.emplace_back(name);

        return m_computeNameStorage.back().c_str();
    }

#endif
}
