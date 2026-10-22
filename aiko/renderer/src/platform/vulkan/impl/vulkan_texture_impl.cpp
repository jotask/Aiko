#include "vulkan_texture_impl.h"

#include <algorithm>
#include <random>
#include <cstring>

#include <intrumentor/profiler.h>
#include <logger/logger.h>
#include <core/file.h>

#include "platform/vulkan/vulkan_context.h"
#include "platform/vulkan/vulkan_types.h"

namespace aiko::renderer::vulkan
{

    VulkanTextureImpl::VulkanTextureImpl()
    {
    }

    VulkanTextureImpl::~VulkanTextureImpl()
    {
        AIKO_ASSERT(m_view == VK_NULL_HANDLE && m_image == VK_NULL_HANDLE && m_memory == VK_NULL_HANDLE, "VulkanTextureImpl destroyed without unload()");
    }

    RenderResourceId VulkanTextureImpl::id() const
    {
        return static_cast<uint>(reinterpret_cast<uintptr_t>(m_view));
    }

    bool VulkanTextureImpl::isValid() const
    {
        return m_image != VK_NULL_HANDLE && m_memory != VK_NULL_HANDLE && m_view != VK_NULL_HANDLE && m_info.valid;
    }

    TextureInfo VulkanTextureImpl::getInfo()
    {
        return m_info;
    }

    void VulkanTextureImpl::create(const TextureDesc& desc)
    {
        unload();

        AIKO_ASSERT(desc.width > 0 && desc.height > 0, "Invalid texture size");

        if (desc.format == TextureFormat::RGBA8_SRGB)
        {
            AIKO_ASSERT(desc.type == TextureType::Sampled && !desc.computeWrite, "sRGB textures must be sampled non-storage textures");
        }

        VulkanContext& ctx = VulkanContext::current();

        m_vkFormat = convertToVkFormat(desc.format);

        const uint32_t maxMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(desc.width, desc.height)))) + 1;

        if (desc.type == TextureType::Sampled)
        {
            AIKO_ASSERT(desc.mipmaps >= 1, "Sampled texture must have at least one mip level");
            AIKO_ASSERT(static_cast<uint32_t>(desc.mipmaps) <= maxMipLevels, "Sampled texture mip count exceeds maximum mip levels");
            m_mipLevels = static_cast<uint32_t>(desc.mipmaps);
        }
        else
        {
            AIKO_ASSERT(desc.mipmaps == 1, "Only sampled textures currently support multiple mip levels");
            m_mipLevels = 1;
        }

        VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT;

        switch (desc.type)
        {
            case TextureType::Sampled:
                usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                if (desc.computeWrite)
                {
                    usage |= VK_IMAGE_USAGE_STORAGE_BIT;
                }
                if (m_mipLevels > 1)
                {
                    usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
                }
                break;

            case TextureType::RenderTarget:
                usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
                if (desc.computeWrite)
                {
                    usage |= VK_IMAGE_USAGE_STORAGE_BIT;
                }
                break;

            case TextureType::DepthStencil:
                usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
                break;

            default:
                AIKO_ASSERT(false, "Unsupported Vulkan texture type");
        }

        VkFormatProperties formatProperties{};
        vkGetPhysicalDeviceFormatProperties(ctx.physicalDevice(), m_vkFormat, &formatProperties);

        VkFormatFeatureFlags requiredFeatures = 0;

        switch (desc.type)
        {
        case TextureType::Sampled:
            requiredFeatures |= VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;

            if (desc.computeWrite)
            {
                requiredFeatures |= VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
            }
            break;

        case TextureType::RenderTarget:
            requiredFeatures |= VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;

            if (desc.computeWrite)
            {
                requiredFeatures |= VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
            }
            break;

        case TextureType::DepthStencil:
            requiredFeatures |= VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
            break;

        default:
            AIKO_ASSERT(false, "Unsupported Vulkan texture type");
        }

        AIKO_ASSERT((formatProperties.optimalTilingFeatures & requiredFeatures) == requiredFeatures, "Texture format does not support the requested Vulkan usage");

        const VkResult result = ctx.createImage(
            static_cast<uint32_t>(desc.width),
            static_cast<uint32_t>(desc.height),
            m_mipLevels,
            m_vkFormat,
            VK_IMAGE_TILING_OPTIMAL,
            usage,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            m_image,
            m_memory
        );

        if (result != VK_SUCCESS)
        {
            logger::Log::error("Failed to create Vulkan texture image (VkResult %d)", static_cast<int>(result));
            unload();
            return;
        }

        m_view = ctx.createImageView(m_image, m_vkFormat, aspect, m_mipLevels);

        m_info =
        {
            .type = desc.type,
            .format = desc.format,
            .width = desc.width,
            .height = desc.height,
            .mipmaps = desc.mipmaps,
            .computeWrite = desc.computeWrite,
            .valid = true,
        };

        m_state =
        {
            .layout = VK_IMAGE_LAYOUT_UNDEFINED,
            .stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            .access = 0,
        };

        m_lifetime = std::make_shared<const u8>(0);

    }

    void VulkanTextureImpl::unload()
    {

        m_lifetime.reset();

        if (m_view == VK_NULL_HANDLE && m_image == VK_NULL_HANDLE && m_memory == VK_NULL_HANDLE)
        {
            m_info = {};
            m_vkFormat = VK_FORMAT_UNDEFINED;
            m_state = {};
            m_mipLevels = 1;
            return;
        }

        const VkImageView view = m_view;
        const VkImage image = m_image;
        const VkDeviceMemory memory = m_memory;

        m_view = VK_NULL_HANDLE;
        m_image = VK_NULL_HANDLE;
        m_memory = VK_NULL_HANDLE;
        m_vkFormat = VK_FORMAT_UNDEFINED;
        m_state = {};
        m_mipLevels = 1;
        m_info = {};

        VulkanContext::current().retireImage(VK_NULL_HANDLE, view, image, memory);

    }

    void VulkanTextureImpl::update(const TextureAsset& asset)
    {
        setPixels(asset.pixels);
    }

    void VulkanTextureImpl::setPixels(const vector<Color>& pixels)
    {
        AIKO_FUNCTION_PROFILE

        AIKO_ASSERT(isValid(), "Invalid texture");
        AIKO_ASSERT(m_info.type == TextureType::Sampled, "setPixels only supports sampled textures for now");
        AIKO_ASSERT(m_info.format == TextureFormat::RGBA8 || m_info.format == TextureFormat::BGRA8 || m_info.format == TextureFormat::RGBA8_SRGB, "Unsupported sampled texture format");

        const size_t pixelCount = size_t(m_info.width) * size_t(m_info.height);

        AIKO_ASSERT(pixels.size() == pixelCount,"Mismatch pixels count and texture size");

        VulkanContext& ctx = VulkanContext::current();

        const bool computeOwned = m_state.queueFamily != VK_QUEUE_FAMILY_IGNORED && m_state.queueFamily != ctx.graphicsQueueFamily();

        const bool hadExistingContents = m_state.layout != VK_IMAGE_LAYOUT_UNDEFINED;

        if (m_state.layout == VK_IMAGE_LAYOUT_GENERAL || computeOwned)
        {
            AIKO_ASSERT(ctx.activeCommandBuffer() == VK_NULL_HANDLE, "Cannot replace compute texture pixels during an active frame yet");
        }

        VkBuffer stagingBuffer = VK_NULL_HANDLE;
        VkDeviceMemory stagingMemory = VK_NULL_HANDLE;
        void* mapped = nullptr;

        const VkDeviceSize imageSize = VkDeviceSize(pixelCount) * 4;

        const VkResult allocationResult = ctx.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingMemory);

        if (allocationResult != VK_SUCCESS)
        {
            logger::Log::error("Failed to allocate texture staging buffer (VkResult %d)", static_cast<int>(allocationResult));

            if (!hadExistingContents)
            {
                unload();
            }

            return;
        }

        const VkResult mapResult = vkMapMemory(ctx.device(), stagingMemory, 0, imageSize, 0, &mapped);

        if (mapResult != VK_SUCCESS)
        {
            logger::Log::error("Failed to map texture staging memory (VkResult %d)", static_cast<int>(mapResult));

            vkDestroyBuffer(ctx.device(), stagingBuffer, nullptr);
            vkFreeMemory(ctx.device(), stagingMemory, nullptr);

            if (!hadExistingContents)
            {
                unload();
            }

            return;
        }

        auto* destination = static_cast<uint8_t*>(mapped);

        const auto toByte = [](float value) -> uint8_t
        {
            return static_cast<uint8_t>(std::clamp(value, 0.0f, 1.0f) * 255.0f);
        };

        const bool bgra = m_info.format == TextureFormat::BGRA8;

        for (size_t i = 0; i < pixelCount; ++i)
        {
            const Color& color = pixels[i];

            const uint8_t r = toByte(color.r);
            const uint8_t g = toByte(color.g);
            const uint8_t b = toByte(color.b);
            const uint8_t a = toByte(color.a);

            const size_t offset = i * 4;

            destination[offset + 0] = bgra ? b : r;
            destination[offset + 1] = g;
            destination[offset + 2] = bgra ? r : b;
            destination[offset + 3] = a;
        }

        vkUnmapMemory(ctx.device(), stagingMemory);

        if (computeOwned)
        {
            AIKO_ASSERT(ctx.hasDedicatedComputeQueue() && m_state.queueFamily == ctx.computeQueueFamily(), "Texture has unexpected queue ownership");
            ctx.waitIdle();
            m_state = {};
        }

        VkCommandBuffer commandBuffer = ctx.beginSingleTimeCommands();

        if (m_state.layout != VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            ctx.transitionImageLayout(commandBuffer, m_image, m_vkFormat, m_state.layout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 0, m_mipLevels);
        }

        ctx.copyBufferToImage(commandBuffer, stagingBuffer, m_image, m_info.width, m_info.height);

        if (m_mipLevels > 1)
        {
            ctx.generateMipmaps(commandBuffer, m_image, m_vkFormat, m_info.width, m_info.height, m_mipLevels);
        }
        else
        {
            ctx.transitionImageLayout(commandBuffer, m_image, m_vkFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }

        ctx.endSingleTimeCommands(commandBuffer);

        m_state =
        {
            .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            .access = VK_ACCESS_SHADER_READ_BIT,
            .queueFamily = ctx.graphicsQueueFamily(),
        };

        vkDestroyBuffer(ctx.device(), stagingBuffer, nullptr);
        vkFreeMemory(ctx.device(), stagingMemory, nullptr);
    }

    VkFormat VulkanTextureImpl::convertToVkFormat(const TextureFormat format)
    {
        switch (format)
        {
            case TextureFormat::BGRA8:          return VkFormat::VK_FORMAT_B8G8R8A8_UNORM;
            case TextureFormat::RGBA8:          return VkFormat::VK_FORMAT_R8G8B8A8_UNORM;
            case TextureFormat::D24S8:          return VkFormat::VK_FORMAT_D24_UNORM_S8_UINT;
            case TextureFormat::D32F:           return VkFormat::VK_FORMAT_D32_SFLOAT;
            case TextureFormat::RGBA8_SRGB:     return VK_FORMAT_R8G8B8A8_SRGB;
            case TextureFormat::RGBA16F:        return VK_FORMAT_R16G16B16A16_SFLOAT;
            case TextureFormat::INVALID:
            default:
                AIKO_ASSERT(false, "Texture format not supported!");
                return VK_FORMAT_UNDEFINED;
        }
    }


}
