#pragma once

#include <cstdint>
#include <vector>

#include <volk.h>

#include "vulkan_render_types.h"
#include "vulkan_types.h"

namespace aiko::renderer::vulkan
{
    class VulkanContext;

    class VulkanFrameResources final
    {
    public:
        VulkanFrameResources(const VulkanFrameResources&) = delete;
        VulkanFrameResources& operator=(const VulkanFrameResources&) = delete;
        VulkanFrameResources(VulkanFrameResources&&) = delete;
        VulkanFrameResources& operator=(VulkanFrameResources&&) = delete;

        VulkanFrameResources( VulkanContext& context, uint32_t frameCount);

        void create();
        void destroy();

        void resetFrame(uint32_t frameIndex);

        const VulkanFrameBinding& allocate(uint32_t frameIndex, const VulkanFrameUbo& ubo);

        const VulkanFrameBinding& latest(uint32_t frameIndex) const;

        VkDescriptorSetLayout layout() const { return m_layout; }

    private:
        static constexpr uint32_t  MaxBindingsPerFrame = 32;

        void recycleBindings(uint32_t frameIndex);
        void destroyBindings(uint32_t frameIndex);

        VulkanContext& m_context;

        std::vector<VkDescriptorPool> m_pools;

        std::vector<std::vector<VulkanFrameBinding>> m_bindings;

        std::vector<std::vector<VulkanFrameBinding>> m_recycledBindings;

        VkDescriptorSetLayout m_layout = VK_NULL_HANDLE;
    };
}
