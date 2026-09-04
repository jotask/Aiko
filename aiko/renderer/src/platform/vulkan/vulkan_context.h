#pragma once

#include "vulkan_types.h"

#include <array>
#include <optional>
#include <limits>

namespace aiko::renderer::vulkan
{
    class VulkanContext
    {
    public:

        static VulkanContext& current();

        void init(const DeviceInitDesc&);
        void shutdown();

        VkInstance instance() const { return m_vk; }
        VkDevice device() const { return m_device; }
        VkPhysicalDevice physicalDevice() const { return m_physicalDevice; }
        VkQueue graphicsQueue() const { return m_graphicsQueue; }
        VkRenderPass renderPass() const { return m_renderPass; }
        std::vector<VkImage> swapChainImages() const { return m_swapChainImages; }
        VkFormat swapChainImageFormat() const { return m_swapChainImageFormat; }
        VkFormat depthFormat() const;
        VkExtent2D swapChainExtent() const { return m_swapChainExtent; }
        VkFramebuffer currentSwapChainFramebuffer() const { return m_swapChainFramebuffers[m_currentImageIndex]; }
        VkCommandBuffer activeCommandBuffer() const { return m_activeCommandBuffer; }
        VkCommandBuffer preComputeCommandBuffer();
        VkCommandBuffer computeCommandBuffer();
        VkSwapchainKHR swapChain() const { return m_swapChain; }
        VkQueue presentQueue() const { return m_presentQueue; }
        uint32_t currentFrameIndex() const { return m_currentFrame; }
        VkQueue computeQueue() const { return m_computeQueue; }
        uint32_t graphicsQueueFamily() const { return m_graphicsQueueFamily; }
        uint32_t computeQueueFamily() const { return m_computeQueueFamily; }
        bool hasDedicatedComputeQueue() const { return m_computeQueue != VK_NULL_HANDLE && m_computeQueueFamily != m_graphicsQueueFamily; }

    private:

        static constexpr size_t MAX_FRAMES_IN_FLIGHT = 2;

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

        std::array<RetiredResources, MAX_FRAMES_IN_FLIGHT> m_retiredResources;
        std::optional<uint32_t> m_lastSubmittedFrame;

        uint32_t retirementFrameIndex() const;
        void destroyRetiredResourcesForFrame(uint32_t frameIndex);
        void destroyRetiredResources();

        VkInstance m_vk = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
        VkSurfaceKHR m_surface = VK_NULL_HANDLE;
        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
        VkDevice m_device = VK_NULL_HANDLE;
        VkRenderPass m_renderPass = VK_NULL_HANDLE;
        VkCommandPool m_commandPool = VK_NULL_HANDLE;
        VkCommandPool m_computeCommandPool = VK_NULL_HANDLE;

        VkQueue m_graphicsQueue = VK_NULL_HANDLE;
        VkQueue m_computeQueue = VK_NULL_HANDLE;
        VkQueue m_presentQueue = VK_NULL_HANDLE;

        uint32_t m_graphicsQueueFamily = std::numeric_limits<uint32_t>::max();
        uint32_t m_computeQueueFamily = std::numeric_limits<uint32_t>::max();
        uint32_t m_presentQueueFamily = std::numeric_limits<uint32_t>::max();

        VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
        std::vector<VkImage> m_swapChainImages;
        std::vector<VkImageView> m_swapChainImageViews;
        VkFormat m_swapChainImageFormat = VK_FORMAT_UNDEFINED;
        VkExtent2D m_swapChainExtent = {};
        bool m_swapChainFormatChanged = false;

        std::vector<VkSemaphore> m_renderFinishedSemaphores;
        std::vector<VkSemaphore> m_imageAvailableSemaphores;
        std::vector<VkSemaphore> m_computeFinishedSemaphores;
        std::vector<VkSemaphore> m_graphicsToComputeSemaphores;
        std::vector<VkFence> m_inFlightFences;

        std::vector<VkImage> m_swapChainDepthImages;
        std::vector<VkDeviceMemory> m_swapChainDepthMemories;
        std::vector<VkImageView> m_swapChainDepthViews;

        std::vector<VkFramebuffer> m_swapChainFramebuffers;
        std::vector<VkCommandBuffer> m_commandBuffers;
        std::vector<VkCommandBuffer> m_preComputeCommandBuffers;
        std::vector<VkCommandBuffer> m_computeCommandBuffers;

        bool m_preComputeCommandBufferUsed = false;
        bool m_computeCommandBufferUsed = false;

        uint32_t m_currentFrame = 0;
        uint32_t m_currentImageIndex = 0;
        VkCommandBuffer m_activeCommandBuffer = VK_NULL_HANDLE;

        bool m_framebufferResized = false;
        bool m_vsync = false;

    public:

        void createInstance();
        void setupDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createSwapChain(VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE);
        void createImageViews();
        void createRenderPass();
        void createCommandPool();
        void createSyncObjects();
        void createSwapChainDepthResources();
        void createSwapChainFramebuffers();
        void createCommandBuffers();
        void createComputeCommandPool();
        void createComputeCommandBuffers();
        void createPreComputeCommandBuffers();

        bool beginFrame();
        void submitAndPresent();

        void recreateSwapChain();
        void cleanupSwapChainResources();
        void cleanupSwapChain();

        void requestSwapChainRecreation();

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes, bool vsync);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels = 1);
        bool isDeviceSuitable(VkPhysicalDevice device);

        void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        void generateMipmaps(VkImage image, VkFormat format, uint32_t width, uint32_t height, uint32_t mipLevel);
        void generateMipmaps(VkCommandBuffer commandBuffer, VkImage image, VkFormat format, uint32_t width, uint32_t height, uint32_t mipLevels);

        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory, VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE, const uint32_t* queueFamilyIndices = nullptr, uint32_t queueFamilyIndexCount = 0);
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void copyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t baseMipLevel = 0, uint32_t levelCount = 1);
        void transitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t baseMipLevel = 0, uint32_t levelCount = 1, bool computeQueue = false);
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
        void copyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
        void retireBuffer(VkBuffer buffer, VkDeviceMemory memory);
        void retireImage(VkSampler sampler, VkImageView view, VkImage image, VkDeviceMemory memory);
        void retireFrameBuffer(VkFramebuffer framebuffer, VkRenderPass renderPass);

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);

        bool consumeSwapChainFormatChanged();

        void waitIdle();

    };
}
