#include "vulkan_context.h"

#include "vulkan_types.h"
#include "display/display_manager.h"

#include <aiko_types.h>

#include <platform/vulkan/vulkan_platform_helper.h>

#include <array>
#include <algorithm>
#include <limits>
#include <set>
#include <stdexcept>

namespace aiko::renderer::vulkan
{

    namespace
    {
        VulkanContext* s_currentContext = nullptr;
    }

    VulkanContext& VulkanContext::current()
    {
        AIKO_ASSERT(s_currentContext != nullptr, "VulkanContext::current called before VulkanContext::init");
        return *s_currentContext;
    }

    void VulkanContext::init(const DeviceInitDesc& desc)
    {

        AIKO_ASSERT(s_currentContext == nullptr || s_currentContext == this, "Multiple VulkanContext instances are active");
        s_currentContext = this;

        m_vsync = desc.vsync;

        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createRenderPass();
        createCommandPool();
        createComputeCommandPool();
        createSwapChainDepthResources();
        createSwapChainFramebuffers();
        createCommandBuffers();
        createSyncObjects();
    }

    void VulkanContext::shutdown()
    {
        if (m_device != VK_NULL_HANDLE)
        {
            vkDeviceWaitIdle(m_device);
        }

        if (m_device != VK_NULL_HANDLE)
        {
            cleanupSwapChain();
        }

        for (VkSemaphore semaphore : m_imageAvailableSemaphores)
        {
            if (semaphore != VK_NULL_HANDLE)
            {
                vkDestroySemaphore(m_device, semaphore, nullptr);
            }
        }
        m_imageAvailableSemaphores.clear();

        for (VkSemaphore semaphore : m_computeFinishedSemaphores)
        {
            if (semaphore != VK_NULL_HANDLE)
            {
                vkDestroySemaphore(m_device, semaphore, nullptr);
            }
        }
        m_computeFinishedSemaphores.clear();

        for (VkFence fence : m_inFlightFences)
        {
            if (fence != VK_NULL_HANDLE)
            {
                vkDestroyFence(m_device, fence, nullptr);
            }
        }
        m_inFlightFences.clear();

        for (VkFence fence : m_computeInFlightFences)
        {
            if (fence != VK_NULL_HANDLE)
            {
                vkDestroyFence(m_device, fence, nullptr);
            }
        }
        m_computeInFlightFences.clear();

        if (m_commandPool != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(m_device, m_commandPool, nullptr);
            m_commandPool = VK_NULL_HANDLE;
        }

        m_commandBuffers.clear();
        m_activeCommandBuffer = VK_NULL_HANDLE;

        if (m_computeCommandPool != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(m_device, m_computeCommandPool,  nullptr);
            m_computeCommandPool = VK_NULL_HANDLE;
        }

        if (m_renderPass != VK_NULL_HANDLE)
        {
            vkDestroyRenderPass(m_device, m_renderPass, nullptr);
            m_renderPass = VK_NULL_HANDLE;
        }

        if (m_device != VK_NULL_HANDLE)
        {
            vkDestroyDevice(m_device, nullptr);
            m_device = VK_NULL_HANDLE;
        }

        m_graphicsQueue = VK_NULL_HANDLE;
        m_presentQueue = VK_NULL_HANDLE;
        m_computeQueue = VK_NULL_HANDLE;
        m_physicalDevice = VK_NULL_HANDLE;

        if (m_surface != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(m_vk, m_surface, nullptr);
            m_surface = VK_NULL_HANDLE;
        }

        if (m_debugMessenger != VK_NULL_HANDLE)
        {
            DestroyDebugUtilsMessengerEXT(m_vk, m_debugMessenger, nullptr );
            m_debugMessenger = VK_NULL_HANDLE;
        }

        if (m_vk != VK_NULL_HANDLE)
        {
            vkDestroyInstance(m_vk, nullptr);
            m_vk = VK_NULL_HANDLE;
        }

        m_currentFrame = 0;
        m_currentImageIndex = 0;

        if (s_currentContext == this)
        {
            s_currentContext = nullptr;
        }
    }

    void VulkanContext::createInstance()
    {
        if constexpr (enableValidationLayers)
        {
            AIKO_ASSERT(checkValidationLayerSupport() == true, "validation layers requested, but not available!");
        }

        constexpr VkApplicationInfo appInfo =
        {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = "Naiko Renderer",
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "Aiko",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_API_VERSION_1_0,
        };

        const std::vector<const char*> extensions = getRequiredExtensions();

        if constexpr (enableValidationLayers == true)
        {
            constexpr VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo =
            {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                .pfnUserCallback = s_debugCallback,
            };

            const VkInstanceCreateInfo createInfo =
            {
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .pNext = &debugCreateInfo,
                .pApplicationInfo = &appInfo,
                .enabledLayerCount = static_cast<uint32_t>(validationLayers.size()),
                .ppEnabledLayerNames = validationLayers.data(),
                .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
                .ppEnabledExtensionNames = extensions.data(),
            };

            const VkResult result = vkCreateInstance( &createInfo, nullptr, &m_vk);
            AIKO_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan instance!");

        }
        else
        {
            const VkInstanceCreateInfo createInfo =
            {
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .pNext = nullptr,
                .pApplicationInfo = &appInfo,
                .enabledLayerCount = 0,
                .ppEnabledLayerNames = nullptr,
                .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
                .ppEnabledExtensionNames = extensions.data(),
            };

            const VkResult result = vkCreateInstance( &createInfo, nullptr, &m_vk);
            AIKO_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan instance!");

        }


        // -----------------------------------
        // Required because we're using Volk
        // -----------------------------------
        volkLoadInstance(m_vk);

    }

    void VulkanContext::setupDebugMessenger()
    {
        if constexpr (enableValidationLayers == false)
        {
            return;
        }

        const VkDebugUtilsMessengerCreateInfoEXT createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = s_debugCallback,
        };

        if (createDebugUtilsMessengerEXT(m_vk, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
        {
            logger::Log::critical("Failed to set up debug messenger!");
            std::exit(-1);
        }
    }

    void VulkanContext::createSurface()
    {
        GLFWwindow* window = DisplayManager::it().getNativeWindow();
        const VkResult result = glfwCreateWindowSurface(m_vk, window, nullptr, &m_surface);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create window surface!");
    }

    void VulkanContext::pickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        const VkResult result =vkEnumeratePhysicalDevices(m_vk, &deviceCount, nullptr);
        AIKO_ASSERT(result == VK_SUCCESS, "vkEnumeratePhysicalDevices failed");
        logger::Log::info("Physical Devices found : %u", deviceCount);
        AIKO_ASSERT(deviceCount > 0, "failed to find GPUs with Vulkan support!");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        const VkResult res = vkEnumeratePhysicalDevices(m_vk, &deviceCount, devices.data());
        AIKO_ASSERT(res == VK_SUCCESS, "Failed to enumerate physical devices");

        for (const auto& device : devices)
        {
            VkPhysicalDeviceProperties properties{};
            vkGetPhysicalDeviceProperties(device, &properties);
            logger::Log::info("Found GPU %s", properties.deviceName);
            if (isDeviceSuitable(device) == true)
            {
                m_physicalDevice = device;
                break;
            }
        }
        AIKO_ASSERT(m_physicalDevice != VK_NULL_HANDLE, "failed to find a suitable GPU!");
    }

    void VulkanContext::createLogicalDevice()
    {
        QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

        std::set<uint32_t> uniqueQueueFamilies = {
            indices.graphicsFamily.value(),
            indices.presentFamily.value(),
            indices.computeFamily.value(),
        };

        float queuePriority = 1.0f;
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

        for (uint32_t queuefamily : uniqueQueueFamilies)
        {
            const VkDeviceQueueCreateInfo queueInfo =
            {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = queuefamily,
                .queueCount = 1,
                .pQueuePriorities = &queuePriority,
            };
            queueCreateInfos.push_back(queueInfo);
        }

        const VkPhysicalDeviceFeatures deviceFeatures = { };

        const VkDeviceCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
            .pQueueCreateInfos = queueCreateInfos.data(),
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
            .ppEnabledExtensionNames = deviceExtensions.data(),
            .pEnabledFeatures = &deviceFeatures,
        };

        const VkResult result = vkCreateDevice( m_physicalDevice, &createInfo, nullptr, &m_device);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create logical device!");

        // Because you're using Volk
        volkLoadDevice(m_device);

        // Retrieve the queues
        vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
        vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue );
        vkGetDeviceQueue(m_device, indices.computeFamily.value(), 0, &m_computeQueue);

        logger::Log::info("Logical Device Initialized");

    }

    void VulkanContext::createSwapChain(VkSwapchainKHR oldSwapchain)
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_physicalDevice, m_surface);

        const VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes, m_vsync);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilties);

        uint32_t imageCount = swapChainSupport.capabilties.minImageCount + 1;
        if (swapChainSupport.capabilties.maxImageCount > 0 && imageCount > swapChainSupport.capabilties.maxImageCount)
        {
            imageCount = swapChainSupport.capabilties.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface = m_surface,
            .minImageCount = imageCount,
            .imageFormat = surfaceFormat.format,
            .imageColorSpace = surfaceFormat.colorSpace,
            .imageExtent = extent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        };

        QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);
        uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        if (indices.graphicsFamily != indices.presentFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapChainSupport.capabilties.currentTransform;

        constexpr std::array<VkCompositeAlphaFlagBitsKHR, 4> compositeAlphaModes =
        {
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR
        };

        for (const VkCompositeAlphaFlagBitsKHR mode : compositeAlphaModes)
        {
            if (swapChainSupport.capabilties.supportedCompositeAlpha & mode)
            {
                createInfo.compositeAlpha = mode;
                break;
            }
        }

        createInfo.presentMode = presentMode;

        createInfo.clipped = VK_TRUE;

        createInfo.oldSwapchain = oldSwapchain;

        VkResult swapChainResult = vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapChain);
        AIKO_ASSERT(swapChainResult == VK_SUCCESS, "Failed to crate SwapChain" );

        vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, nullptr);
        m_swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, m_swapChainImages.data());

        m_swapChainImageFormat = surfaceFormat.format;
        m_swapChainExtent = extent;
    }

    void VulkanContext::createImageViews()
    {
        m_swapChainImageViews.resize(m_swapChainImages.size());
        for (uint32_t i = 0; i < m_swapChainImages.size(); i++)
        {
            m_swapChainImageViews[i] = createImageView(m_swapChainImages[i], m_swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
        }
    }

    void VulkanContext::createRenderPass()
    {
        const VkAttachmentDescription colorAttachment =
        {
            .format = m_swapChainImageFormat,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        };

        const VkAttachmentReference colorAttachmentRef =
        {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };

        const VkAttachmentDescription depthAttachment =
        {
            .format = findDepthFormat(m_physicalDevice),
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        };

        const VkAttachmentReference depthAttachmentRef =
        {
            .attachment = 1,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        };

        const VkSubpassDescription subpass =
        {
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachmentRef,
            .pDepthStencilAttachment = &depthAttachmentRef,
        };

        const VkSubpassDependency dependency =
        {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            .srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        };

        std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

        const VkRenderPassCreateInfo renderPassInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = static_cast<uint32_t>(attachments.size()),
            .pAttachments = attachments.data(),
            .subpassCount = 1,
            .pSubpasses = &subpass,
            .dependencyCount = 1,
            .pDependencies = &dependency,
        };

        VkResult result = vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create render pass!");
    }

    void VulkanContext::createCommandPool()
    {
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_physicalDevice);

        const VkCommandPoolCreateInfo poolInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = queueFamilyIndices.graphicsFamily.value(),
        };

        if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create command pool!");
        }
    }

    void VulkanContext::createSyncObjects()
    {
        m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        m_renderFinishedSemaphores.resize(m_swapChainImages.size());
        m_computeFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_computeInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        const VkSemaphoreCreateInfo semaphoreInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };

        const VkFenceCreateInfo fenceInfo
        {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        };

        // Presentation semaphores:
        // one per swapchain image.
        for (size_t i = 0; i < m_swapChainImages.size(); ++i)
        {
            if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i] ) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create render finished semaphore!");
            }
        }

        // Frame resources:
        // one per frame in flight.
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS ||
                vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_computeFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(m_device, &fenceInfo, nullptr, &m_computeInFlightFences[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create frame synchronization objects!");
            }
        }
    }

    void VulkanContext::createSwapChainDepthResources()
    {
        const VkFormat depthFormat = findDepthFormat(m_physicalDevice);

        const size_t imageCount = m_swapChainImages.size();

        m_swapChainDepthImages.resize(imageCount, VK_NULL_HANDLE);
        m_swapChainDepthMemories.resize(imageCount, VK_NULL_HANDLE);
        m_swapChainDepthViews.resize(imageCount, VK_NULL_HANDLE);

        for (size_t i = 0; i < imageCount; ++i)
        {
            createImage(m_swapChainExtent.width, m_swapChainExtent.height, 1, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_swapChainDepthImages[i], m_swapChainDepthMemories[i]);
            m_swapChainDepthViews[i] = createImageView(m_swapChainDepthImages[i], depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
        }
    }

    void VulkanContext::createSwapChainFramebuffers()
    {
        m_swapChainFramebuffers.resize(m_swapChainImageViews.size());

        for (size_t i = 0; i < m_swapChainImageViews.size(); ++i)
        {
            std::array<VkImageView, 2> attachments =
            {
                m_swapChainImageViews[i],
                m_swapChainDepthViews[i]
            };

            const VkFramebufferCreateInfo framebufferInfo =
            {
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .renderPass = m_renderPass,
                .attachmentCount = static_cast<uint32_t>(attachments.size()),
                .pAttachments = attachments.data(),
                .width = m_swapChainExtent.width,
                .height = m_swapChainExtent.height,
                .layers = 1,
            };

            const VkResult resultCreateBuffer = vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_swapChainFramebuffers[i]);
            AIKO_ASSERT(resultCreateBuffer == VK_SUCCESS, "Failed to create framebuffer");
        }
    }

    void VulkanContext::createCommandBuffers()
    {
        m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        const VkCommandBufferAllocateInfo allocInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = m_commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size()),
        };

        const VkResult resultAllocation = vkAllocateCommandBuffers(m_device, &allocInfo, m_commandBuffers.data());
        AIKO_ASSERT(resultAllocation == VK_SUCCESS, "Failed to allocate command buffer");
    }

    bool VulkanContext::beginFrame()
    {

        if (m_framebufferResized)
        {
            recreateSwapChain();
        }

        vkWaitForFences(m_device, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex = 0;
        VkResult acquireResult = vkAcquireNextImageKHR(m_device, m_swapChain, UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &imageIndex );

        if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            acquireResult = vkAcquireNextImageKHR(m_device, m_swapChain, UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &imageIndex);
        }

        AIKO_ASSERT(acquireResult == VK_SUCCESS || acquireResult == VK_SUBOPTIMAL_KHR, "Failed to acquire swapchain image");

        m_currentImageIndex = imageIndex;

        vkResetFences(m_device, 1, &m_inFlightFences[m_currentFrame]);

        m_activeCommandBuffer = m_commandBuffers[m_currentFrame];

        vkResetCommandBuffer(m_activeCommandBuffer, 0);

        const VkCommandBufferBeginInfo beginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        };

        const VkResult beginResult = vkBeginCommandBuffer(m_activeCommandBuffer, &beginInfo);
        AIKO_ASSERT(beginResult == VK_SUCCESS, "Failed to begin command buffer");

        return beginResult == VK_SUCCESS;
    }

    void VulkanContext::submitAndPresent()
    {
        const VkResult endResult = vkEndCommandBuffer(m_activeCommandBuffer);
        AIKO_ASSERT(endResult == VK_SUCCESS, "Failed to end command buffer");

        const VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame], m_computeFinishedSemaphores[m_currentFrame] };
        const VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        const VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentImageIndex] };

        const VkSubmitInfo submitInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = waitSemaphores,
            .pWaitDstStageMask = waitStages,
            .commandBufferCount = 1,
            .pCommandBuffers = &m_activeCommandBuffer,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = signalSemaphores,
        };

        const VkResult submitResult = vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame]);
        AIKO_ASSERT(submitResult == VK_SUCCESS, "Failed to submit graphics command buffer");

        const VkSwapchainKHR swapChains[] = { m_swapChain };

        const VkPresentInfoKHR presentInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = signalSemaphores,
            .swapchainCount = 1,
            .pSwapchains = swapChains,
            .pImageIndices = &m_currentImageIndex,
        };

        const VkResult presentResult = vkQueuePresentKHR(m_presentQueue, &presentInfo);

        m_activeCommandBuffer = VK_NULL_HANDLE;

        if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR)
        {
            recreateSwapChain();
            return;
        }

        AIKO_ASSERT(presentResult == VK_SUCCESS, "Failed to present swapchain image");

        m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        m_activeCommandBuffer = VK_NULL_HANDLE;
    }

    void VulkanContext::recreateSwapChain()
    {
        GLFWwindow* window = DisplayManager::it().getNativeWindow();

        int width = 0;
        int height = 0;
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(m_device);

        const VkFormat previousFormat = m_swapChainImageFormat;
        const VkSwapchainKHR oldSwapchain = m_swapChain;

        cleanupSwapChainResources();
        createSwapChain(oldSwapchain);

        if (oldSwapchain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(m_device, oldSwapchain, nullptr);
        }

        createImageViews();

        const bool formatChanged = previousFormat != VK_FORMAT_UNDEFINED && previousFormat != m_swapChainImageFormat;

        if (formatChanged == true)
        {
            if (m_renderPass != VK_NULL_HANDLE)
            {
                vkDestroyRenderPass(m_device, m_renderPass, nullptr);
                m_renderPass = VK_NULL_HANDLE;
            }
            createRenderPass();
            m_swapChainFormatChanged = true;
        }

        createSwapChainDepthResources();
        createSwapChainFramebuffers();

        m_renderFinishedSemaphores.resize(m_swapChainImages.size());

        const VkSemaphoreCreateInfo semaphoreInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };

        for (size_t i = 0; i < m_renderFinishedSemaphores.size(); ++i)
        {
            const VkResult result = vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]);
            AIKO_ASSERT(result == VK_SUCCESS, "Failed to recreate render finished semaphore");
        }

        m_currentFrame = 0;
        m_currentImageIndex = 0;
        m_activeCommandBuffer = VK_NULL_HANDLE;

        m_framebufferResized = false;

    }

    void VulkanContext::cleanupSwapChainResources()
    {

        for (VkFramebuffer framebuffer : m_swapChainFramebuffers)
        {
            if (framebuffer != VK_NULL_HANDLE)
            {
                vkDestroyFramebuffer(m_device, framebuffer, nullptr);
            }
        }
        m_swapChainFramebuffers.clear();

        for (VkImageView view : m_swapChainDepthViews)
        {
            if (view != VK_NULL_HANDLE)
            {
                vkDestroyImageView(m_device, view, nullptr);
            }
        }
        m_swapChainDepthViews.clear();

        for (VkImage image : m_swapChainDepthImages)
        {
            if (image != VK_NULL_HANDLE)
            {
                vkDestroyImage(m_device, image, nullptr);
            }
        }
        m_swapChainDepthImages.clear();

        for (VkDeviceMemory memory : m_swapChainDepthMemories)
        {
            if (memory != VK_NULL_HANDLE)
            {
                vkFreeMemory(m_device, memory, nullptr);
            }
        }
        m_swapChainDepthMemories.clear();

        for (VkImageView imageView : m_swapChainImageViews)
        {
            if (imageView != VK_NULL_HANDLE)
            {
                vkDestroyImageView(m_device, imageView, nullptr);
            }
        }

        m_swapChainImageViews.clear();
        m_swapChainImages.clear();

        for (VkSemaphore semaphore : m_renderFinishedSemaphores)
        {
            if (semaphore != VK_NULL_HANDLE)
            {
                vkDestroySemaphore(m_device, semaphore, nullptr);
            }
        }

        m_renderFinishedSemaphores.clear();

    }

    void VulkanContext::cleanupSwapChain()
    {
        cleanupSwapChainResources();
        if (m_swapChain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
            m_swapChain = VK_NULL_HANDLE;
        }
    }

    void VulkanContext::requestSwapChainRecreation()
    {
        m_framebufferResized = true;
    }

    QueueFamilyIndices VulkanContext::findQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(
            device,
            &queueFamilyCount,
            nullptr
        );

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

        vkGetPhysicalDeviceQueueFamilyProperties(device,&queueFamilyCount, queueFamilies.data());

        std::optional<uint32_t> computeFallback;

        for (uint32_t i = 0; i < queueFamilyCount; ++i)
        {
            const VkQueueFamilyProperties& queueFamily = queueFamilies[i];

            if (queueFamily.queueCount == 0)
            {
                continue;
            }

            // -------------------------------------------------------------
            // Graphics queue
            // -------------------------------------------------------------

            if (!indices.graphicsFamily.has_value() && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
            {
                indices.graphicsFamily = i;
            }

            // -------------------------------------------------------------
            // Compute queue
            //
            // Prefer a dedicated compute queue that does not also perform
            // graphics work.
            // -------------------------------------------------------------

            if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
                if (!(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
                {
                    if (indices.computeFamily.has_value() == false)
                    {
                        indices.computeFamily = i;
                    }
                }
                else if (computeFallback.has_value() == false)
                {
                    computeFallback = i;
                }
            }

            // -------------------------------------------------------------
            // Presentation queue
            // -------------------------------------------------------------

            VkBool32 presentSupport = VK_FALSE;

            const VkResult presentResult = vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport );
            AIKO_ASSERT(presentResult == VK_SUCCESS, "Failed to query surface support!");

            if (indices.presentFamily.has_value() == false && presentSupport == true)
            {
                indices.presentFamily = i;
            }
        }

        if (indices.computeFamily.has_value() == false)
        {
            indices.computeFamily = computeFallback;
        }

        return indices;
    }

    void VulkanContext::createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
    {
        const VkImageCreateInfo imageInfo =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = format,
            .extent = VkExtent3D{
                .width = width,
                .height = height,
                .depth = 1,
            },
            .mipLevels = mipLevels,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = tiling,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        if (vkCreateImage(m_device, &imageInfo, nullptr, &image) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(m_device, image, &memRequirements);

        const uint32_t memoryType = findMemoryType(memRequirements.memoryTypeBits, properties);
        AIKO_ASSERT(memoryType != InvalidMemoryType, "Failed to find memory type.");

        const VkMemoryAllocateInfo allocInfo =
        {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = memRequirements.size,
            .memoryTypeIndex = memoryType,
        };

        if (vkAllocateMemory(m_device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(m_device, image, imageMemory, 0);

    }

    void VulkanContext::generateMipmaps(VkImage image, VkFormat format, uint32_t width, uint32_t height, uint32_t mipLevels)
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();
        generateMipmaps(commandBuffer, image, format, width, height, mipLevels);
        endSingleTimeCommands(commandBuffer);
    }

    void VulkanContext::generateMipmaps(VkCommandBuffer commandBuffer, VkImage image, VkFormat format, uint32_t width, uint32_t height, uint32_t mipLevels)
    {

        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &formatProperties );

        AIKO_ASSERT(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT, "Texture format does not support linear blitting" );

        int32_t mipWidth = static_cast<int32_t>(width);
        int32_t mipHeight = static_cast<int32_t>(height);

        for (uint32_t i = 1; i < mipLevels; ++i)
        {
            VkImageMemoryBarrier barrier = {};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.image = image;

            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

            VkImageBlit blit = {};

            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;

            blit.srcOffsets[0] = {0, 0, 0};
            blit.srcOffsets[1] = {mipWidth, mipHeight, 1};

            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            blit.dstOffsets[0] = {0, 0, 0};
            blit.dstOffsets[1] =
            {
                std::max(mipWidth / 2, 1),
                std::max(mipHeight / 2, 1),
                1
            };

            vkCmdBlitImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,&blit, VK_FILTER_LINEAR );

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

            mipWidth = std::max(mipWidth / 2, 1);
            mipHeight = std::max(mipHeight / 2, 1);
        }

        VkImageMemoryBarrier finalBarrier = {};
        finalBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        finalBarrier.image = image;

        finalBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        finalBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        finalBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        finalBarrier.subresourceRange.baseMipLevel = mipLevels - 1;
        finalBarrier.subresourceRange.levelCount = 1;
        finalBarrier.subresourceRange.baseArrayLayer = 0;
        finalBarrier.subresourceRange.layerCount = 1;

        finalBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        finalBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        finalBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        finalBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &finalBarrier);

    }

    void VulkanContext::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
    {
        const VkBufferCreateInfo bufferInfo =
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = size,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        };

        if (vkCreateBuffer(m_device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
        {
            logger::Log::error("Failed to create buffer!");
            std::exit(-1);
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(m_device, buffer, &memRequirements);


        const uint32_t memoryType = findMemoryType(memRequirements.memoryTypeBits, properties);
        AIKO_ASSERT(memoryType != InvalidMemoryType, "Failed to find memory type.");

        const VkMemoryAllocateInfo allocInfo =
        {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = memRequirements.size,
            .memoryTypeIndex = memoryType,
        };

        if (vkAllocateMemory(m_device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(m_device, buffer, bufferMemory, 0);
    }

    void VulkanContext::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();
        copyBuffer(commandBuffer, srcBuffer, dstBuffer, size);
        endSingleTimeCommands(commandBuffer);
    }

    void VulkanContext::copyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
    {
        const VkBufferCopy copyRegion =
        {
            .size = size,
        };
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    }

    void VulkanContext::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t baseMipLevel, uint32_t levelCount)
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();
        transitionImageLayout(commandBuffer, image, format, oldLayout, newLayout, baseMipLevel,levelCount);
        endSingleTimeCommands(commandBuffer);
    }

    void VulkanContext::transitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t baseMipLevel, uint32_t levelCount, bool computeQueue)
    {

        logger::Log::info(
            "Transition image " +
            std::to_string(reinterpret_cast<uintptr_t>(image)) +
            ": " +
            std::string(magic_enum::enum_name(oldLayout)) +
            " -> " +
            std::string(magic_enum::enum_name(newLayout))
        );

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;

        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        barrier.image = image;

        auto hasStencilComponent = [](VkFormat format)
        {
            return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
        };

        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            if (hasStencilComponent(format))
            {
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        }
        else
        {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        barrier.subresourceRange.baseMipLevel = baseMipLevel;

        if (levelCount == 0)
        {
            barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
        }
        else
        {
            barrier.subresourceRange.levelCount = levelCount;
        }

        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_GENERAL)
        {
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_GENERAL && newLayout == VK_IMAGE_LAYOUT_GENERAL)
        {
            barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_GENERAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_GENERAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        }
        else
        {
            logger::Log::error( "Unsupported layout transition: %d -> %d", oldLayout, newLayout );
            std::exit(-1);
        }

        vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    }

    void VulkanContext::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();
        copyBufferToImage(commandBuffer, buffer, image, width, height);
        endSingleTimeCommands(commandBuffer);
    }

    void VulkanContext::copyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
    {
        const VkBufferImageCopy region =
        {
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,

            .imageSubresource =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },

            .imageOffset = {0, 0, 0},
            .imageExtent = {width, height, 1},
        };

        vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    }

    VkCommandBuffer VulkanContext::beginSingleTimeCommands()
    {
        const VkCommandBufferAllocateInfo allocInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = m_commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);

        const VkCommandBufferBeginInfo beginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        };

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void VulkanContext::endSingleTimeCommands(VkCommandBuffer commandBuffer)
    {
        const VkResult result = vkEndCommandBuffer(commandBuffer);

        if (result != VK_SUCCESS)
        {
            logger::Log::error("Failed to end single-time command buffer!");
            std::exit(-1);
        }

        const VkSubmitInfo submitInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
            .pCommandBuffers = &commandBuffer,
        };

        const VkFenceCreateInfo fenceInfo =
        {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        };

        VkFence fence = VK_NULL_HANDLE;

        VkResult resultFence = vkCreateFence(m_device, &fenceInfo, nullptr, &fence);
        AIKO_ASSERT(resultFence == VK_SUCCESS, "Failed to create one-time command fence");

        VkResult resultSubmit = vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, fence);
        AIKO_ASSERT(resultSubmit == VK_SUCCESS, "Failed to submit one-time command buffer");

        VkResult resultWait = vkWaitForFences(m_device, 1, &fence, VK_TRUE, UINT64_MAX);
        AIKO_ASSERT(resultWait == VK_SUCCESS, "Failed waiting for one-time command buffer");

        vkDestroyFence(m_device, fence, nullptr);

        vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
    }

    bool VulkanContext::consumeSwapChainFormatChanged()
    {
        const bool changed = m_swapChainFormatChanged;
        m_swapChainFormatChanged = false;
        return changed;
    }

    uint32_t VulkanContext::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }
        AIKO_ASSERT(false, "failed to find suitable memory type!");
        return InvalidMemoryType;
    }

    VkSurfaceFormatKHR VulkanContext::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR VulkanContext::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes, bool vsync)
    {
        if (vsync ==  true)
        {
            logger::Log::info("Using FIFO present mode (VSync enabled)");
            return VK_PRESENT_MODE_FIFO_KHR;
        }

        for (const VkPresentModeKHR mode : availablePresentModes)
        {
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                logger::Log::info("Using MAILBOX present mode");
                return mode;
            }
        }

        for (const VkPresentModeKHR mode : availablePresentModes)
        {
            if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
            {
                logger::Log::info("Using IMMEDIATE present mode");
                return mode;
            }
        }

        logger::Log::info("Using FIFO present mode");
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanContext::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        GLFWwindow* window = DisplayManager::it().getNativeWindow();
        int width = -1;
        int height = -1;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent =
        {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }

    VkImageView VulkanContext::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
    {
        const VkImageViewCreateInfo viewInfo =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = format,
            .subresourceRange = {
                .aspectMask = aspectFlags,
                .baseMipLevel = 0,
                .levelCount = mipLevels,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        VkImageView imageView;
        if (vkCreateImageView(m_device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
        {
            logger::Log::error("failed to create image view!");
            std::exit(-1);
        }

        return imageView;
    }

    bool VulkanContext::isDeviceSuitable(VkPhysicalDevice device)
    {
        const QueueFamilyIndices indices = findQueueFamilies(device);
        const bool extensionsSupported = checkDeviceExtensionSupport(device);
        bool swapChainAdequate = false;
        if (extensionsSupported)
        {
            const SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, m_surface);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return indices.isComplete() && extensionsSupported && swapChainAdequate;

    }

    void VulkanContext::createComputeCommandPool()
    {
        const QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

        const VkCommandPoolCreateInfo poolInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = indices.computeFamily.value(),
        };

        const VkResult result = vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_computeCommandPool);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create compute command pool");

    }

    VkCommandBuffer VulkanContext::beginComputeCommands()
    {
        const VkCommandBufferAllocateInfo allocInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = m_computeCommandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };

        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

        const VkResult allocResult =
            vkAllocateCommandBuffers( m_device, &allocInfo, &commandBuffer);

        AIKO_ASSERT(allocResult == VK_SUCCESS, "Failed to allocate compute command buffer");

        const VkCommandBufferBeginInfo beginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        };

        const VkResult beginResult = vkBeginCommandBuffer( commandBuffer, &beginInfo);
        AIKO_ASSERT(beginResult == VK_SUCCESS, "Failed to begin compute command buffer");

        return commandBuffer;
    }

    void VulkanContext::endComputeCommands(VkCommandBuffer commandBuffer)
    {

        const VkResult endResult = vkEndCommandBuffer(commandBuffer);
        AIKO_ASSERT(endResult == VK_SUCCESS, "Failed to end compute command buffer");

        const VkSemaphore signalSemaphores[] =
        {
            m_computeFinishedSemaphores[m_currentFrame]
        };

        const VkSubmitInfo submitInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
            .pCommandBuffers = &commandBuffer,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = signalSemaphores,
        };

        const VkFenceCreateInfo fenceInfo =
        {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        };

        VkFence fence = VK_NULL_HANDLE;

        const VkResult fenceResult = vkCreateFence(m_device, &fenceInfo, nullptr, &fence);
        AIKO_ASSERT(fenceResult == VK_SUCCESS, "Failed to create compute fence");

        const VkResult submitResult = vkQueueSubmit(m_computeQueue, 1, &submitInfo, fence);
        AIKO_ASSERT(submitResult == VK_SUCCESS, "Failed to submit compute command buffer");

        const VkResult waitResult = vkWaitForFences( m_device, 1, &fence, VK_TRUE, UINT64_MAX);
        AIKO_ASSERT(waitResult == VK_SUCCESS, "Failed waiting for compute command buffer");

        vkDestroyFence(m_device, fence, nullptr);

        vkFreeCommandBuffers(m_device,m_computeCommandPool,1,&commandBuffer);
    }

}
