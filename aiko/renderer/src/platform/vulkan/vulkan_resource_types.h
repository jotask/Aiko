#pragma once

#include <vector>

#include "vulkan_types.h"

namespace aiko::renderer::vulkan
{
    struct RetiredBuffer
    {
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
    };

    struct RetiredImage
    {
        VkSampler sampler = VK_NULL_HANDLE;
        VkImageView view = VK_NULL_HANDLE;
        VkImage image = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
    };

    struct RetiredFrameBuffer
    {
        VkFramebuffer framebuffer = VK_NULL_HANDLE;
        VkRenderPass renderPass = VK_NULL_HANDLE;
    };

    struct RetiredResources
    {
        std::vector<RetiredBuffer> buffers;
        std::vector<RetiredImage> images;
        std::vector<RetiredFrameBuffer> frameBuffers;
    };
}
