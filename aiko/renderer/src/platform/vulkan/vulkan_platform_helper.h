#pragma once

#include <map>
#include <set>

#include <aiko_types.h>

#include "constants.h"

#include <GLFW/glfw3.h>

#include "vulkan_types.h"


namespace aiko::renderer::vulkan
{

    template<typename T, typename Enumerate> static std::vector<T> enumerateVulkanValues(Enumerate&& enumerate, const char* errorMessage)
    {
        for (;;)
        {
            uint32_t count = 0;

            VkResult result = enumerate(&count, nullptr);
            AIKO_ASSERT(result == VK_SUCCESS, errorMessage);

            if (result != VK_SUCCESS)
            {
                return {};
            }

            if (count == 0)
            {
                return {};
            }

            std::vector<T> values(count);

            result = enumerate(&count, values.data());

            if (result == VK_INCOMPLETE)
            {
                continue;
            }

            AIKO_ASSERT(result == VK_SUCCESS, errorMessage);

            if (result != VK_SUCCESS)
            {
                return {};
            }

            values.resize(count);
            return values;
        }
    }

    static VkBool32 s_debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        std::string msg = "Validation layer: ";
        msg.append(pCallbackData->pMessage);
        switch (messageSeverity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                logger::Log::debug(msg.c_str());
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                logger::Log::info(msg.c_str());
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                logger::Log::warning(msg.c_str());
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                logger::Log::error(msg.c_str());
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
                logger::Log::critical(msg.c_str());
                break;
            default:
                logger::Log::critical(msg.c_str());
                break;
        }
        return VK_FALSE;
    }

    static VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            func(instance, debugMessenger, pAllocator);
        }
    }

    static void checkImGuiVkResult(VkResult res)
    {
        if (res == VK_SUCCESS)
        {
            return;
        }
        logger::Log::error("ImGui Vulkan error: %d", res);
        AIKO_ASSERT(res >= 0, "ImGui Vulkan error!");
    }

    static bool checkValidationLayerSupport()
    {

        const std::vector<VkLayerProperties> availableLayers =
            enumerateVulkanValues<VkLayerProperties>(
                [](uint32_t* count, VkLayerProperties* properties)
                {
                    return vkEnumerateInstanceLayerProperties(count, properties);
                },
                "Failed to enumerate Vulkan layers"
            );

        logger::Log::info("Available Vulkan layers:");

        for (const auto& layer : availableLayers)
        {
            logger::Log::info("\t%s", layer.layerName);
        }

        for (const char* layerName : validationLayers)
        {
            bool layerFound = false;
            for (const auto& layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (layerFound == false)
            {
                logger::Log::error("Missing Vulkan layer: %s", layerName);
                return false;
            }
        }

        return true;
    }

    static std::vector<const char*> getRequiredExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        AIKO_ASSERT(glfwExtensions != nullptr && glfwExtensionCount > 0, "GLFW failed to provide required Vulkan instance extensions");

        if (glfwExtensions == nullptr || glfwExtensionCount == 0)
        {
            return {};
        }

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        if constexpr (enableValidationLayers == true)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        return extensions;
    }

    static bool checkDeviceExtensionSupport(VkPhysicalDevice device)
    {
        const std::vector<VkExtensionProperties> availableExtensions =
            enumerateVulkanValues<VkExtensionProperties>(
                [device](uint32_t* count, VkExtensionProperties* properties)
                {
                    return vkEnumerateDeviceExtensionProperties(
                        device,
                        nullptr,
                        count,
                        properties
                    );
                },
                "Failed to enumerate Vulkan device extensions"
            );

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        SwapChainSupportDetails details;

        const VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilties);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to query Vulkan surface capabilities");

        details.formats = enumerateVulkanValues<VkSurfaceFormatKHR>(
            [device, surface](uint32_t* count, VkSurfaceFormatKHR* formats)
            {
                return vkGetPhysicalDeviceSurfaceFormatsKHR(
                    device,
                    surface,
                    count,
                    formats
                );
            },
            "Failed to query Vulkan surface formats"
        );

        details.presentModes =
            enumerateVulkanValues<VkPresentModeKHR>(
                [device, surface](uint32_t* count, VkPresentModeKHR* presentModes)
                {
                    return vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, count, presentModes);
                },
                "Failed to query Vulkan present modes"
            );

        return details;
    }

    static VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
    {
        for (VkFormat format : candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }
        throw std::runtime_error("failed to find supported format!");
    }

    static VkFormat findDepthFormat(VkPhysicalDevice physicalDevice)
    {
        return findSupportedFormat(physicalDevice,
            {
                    VK_FORMAT_D32_SFLOAT,
                    VK_FORMAT_D32_SFLOAT_S8_UINT,
                    VK_FORMAT_D24_UNORM_S8_UINT
                },
                VK_IMAGE_TILING_OPTIMAL,
                VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
            );
    }

}
