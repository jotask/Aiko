#pragma once

#include "vulkan_types.h"

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
        VkExtent2D swapChainExtent() const { return m_swapChainExtent; }
        VkFramebuffer currentSwapChainFramebuffer() const { return m_swapChainFramebuffers[m_currentImageIndex]; }
        VkCommandBuffer activeCommandBuffer() const { return m_activeCommandBuffer; }
        VkSwapchainKHR swapChain() const { return m_swapChain; }
        VkQueue presentQueue() const { return m_presentQueue; }
        uint32_t currentFrameIndex() const { return m_currentFrame; }

    private:

        const size_t MAX_FRAMES_IN_FLIGHT = 2;

        VkInstance m_vk = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
        VkSurfaceKHR m_surface = VK_NULL_HANDLE;
        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
        VkDevice m_device = VK_NULL_HANDLE;
        VkRenderPass m_renderPass = VK_NULL_HANDLE;
        VkCommandPool m_commandPool = VK_NULL_HANDLE;

        VkQueue m_graphicsQueue = VK_NULL_HANDLE;
        VkQueue m_computeQueue = VK_NULL_HANDLE;
        VkQueue m_presentQueue = VK_NULL_HANDLE;

        VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
        std::vector<VkImage> m_swapChainImages;
        std::vector<VkImageView> m_swapChainImageViews;
        VkFormat m_swapChainImageFormat = VK_FORMAT_UNDEFINED;
        VkExtent2D m_swapChainExtent = {};
        bool m_swapChainFormatChanged = false;

        std::vector<VkSemaphore> m_renderFinishedSemaphores;
        std::vector<VkSemaphore> m_imageAvailableSemaphores;
        std::vector<VkSemaphore> m_computeFinishedSemaphores;
        std::vector<VkFence> m_inFlightFences;
        std::vector<VkFence> m_computeInFlightFences;

        std::vector<VkImage> m_swapChainDepthImages;
        std::vector<VkDeviceMemory> m_swapChainDepthMemories;
        std::vector<VkImageView> m_swapChainDepthViews;

        std::vector<VkFramebuffer> m_swapChainFramebuffers;
        std::vector<VkCommandBuffer> m_commandBuffers;

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

        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t baseMipLevel = 0, uint32_t levelCount = 1);
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);

        bool consumeSwapChainFormatChanged();

    };
}
