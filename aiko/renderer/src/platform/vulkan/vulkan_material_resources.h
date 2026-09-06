#pragma once

#include <vector>

#include <volk.h>

#include "vulkan_render_types.h"

namespace aiko::renderer::vulkan
{
    class VulkanContext;

    class VulkanMaterialResources final
    {
    public:
        VulkanMaterialResources(const VulkanMaterialResources&) = delete;
        VulkanMaterialResources& operator=(const VulkanMaterialResources&) = delete;
        VulkanMaterialResources(VulkanMaterialResources&&) = delete;
        VulkanMaterialResources& operator=(VulkanMaterialResources&&) = delete;

        VulkanMaterialResources(VulkanContext& context, uint32_t frameCount);

        void create(VkDescriptorSetLayout materialLayout);

        void resetFrame(uint32_t frame);

        VulkanMaterialBinding& getOrCreate(uint32_t frame, const MaterialBindingKey& key);

        void destroy();

    private:

        void destroyBindings(uint32_t frame);

        static constexpr uint32_t MaxBindings = 1024;

        VulkanContext& m_context;

        VkDescriptorSetLayout m_materialLayout = VK_NULL_HANDLE;

        std::vector<VulkanMaterialFrameResources> m_frames;
    };
}
