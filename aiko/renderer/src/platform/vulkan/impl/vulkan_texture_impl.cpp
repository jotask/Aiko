#include "vulkan_texture_impl.h"

#include <algorithm>
#include <random>
#include <cstring>

#include <logger/logger.h>
#include <core/file.h>

#include "constants.h"
#include "platform/vulkan/vulkan_context.h"
#include "platform/vulkan/vulkan_types.h"

namespace aiko::renderer::vulkan
{

    VulkanTextureImpl::VulkanTextureImpl()
    {
    }

    VulkanTextureImpl::~VulkanTextureImpl()
    {
        AIKO_ASSERT(m_sampler == VK_NULL_HANDLE && m_view == VK_NULL_HANDLE && m_image == VK_NULL_HANDLE && m_memory == VK_NULL_HANDLE, "VulkanTextureImpl destroyed without unload()");
    }

    RenderResourceId VulkanTextureImpl::id() const
    {
        return static_cast<uint>(reinterpret_cast<uintptr_t>(m_view));
    }

    void VulkanTextureImpl::use()
    {

    }

    void VulkanTextureImpl::unuse()
    {

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

        VulkanContext& ctx = VulkanContext::current();

        m_vkFormat = convertToVkFormat(desc.format);

        VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT;

        switch (desc.type)
        {
            case TextureType::Sampled:
                usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                if (desc.computeWrite)
                {
                    usage |= VK_IMAGE_USAGE_STORAGE_BIT;
                }
                if (desc.mipmaps)
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
                usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
                break;

            default:
                AIKO_ASSERT(false, "Unsupported Vulkan texture type");
        }

        if (desc.type == TextureType::Sampled && desc.mipmaps)
        {
            m_mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(desc.width, desc.height)))) + 1;
        }
        else
        {
            m_mipLevels = 1;
        }

        ctx.createImage(
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

        m_view = ctx.createImageView(m_image, m_vkFormat, aspect, m_mipLevels);

        if (desc.type != TextureType::DepthStencil)
        {
            const VkSamplerCreateInfo samplerInfo =
            {
                .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                .magFilter = VK_FILTER_LINEAR,
                .minFilter = VK_FILTER_LINEAR,
                .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
                .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .mipLodBias = 0.0f,
                .anisotropyEnable = VK_FALSE,
                .maxAnisotropy = 1.0f,
                .compareEnable = VK_FALSE,
                .compareOp = VK_COMPARE_OP_ALWAYS,
                .minLod = 0.0f,
                .maxLod = static_cast<float>(m_mipLevels - 1),
                .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
                .unnormalizedCoordinates = VK_FALSE,
            };

            const VkResult result = vkCreateSampler(ctx.device(), &samplerInfo, nullptr, &m_sampler);
            AIKO_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan texture sampler");
        }

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

    }

    void VulkanTextureImpl::unload()
    {

        if (m_sampler == VK_NULL_HANDLE && m_view == VK_NULL_HANDLE && m_image == VK_NULL_HANDLE && m_memory == VK_NULL_HANDLE)
        {
            m_info = {};
            m_vkFormat = VK_FORMAT_UNDEFINED;
            m_state = {};
            m_mipLevels = 1;
            return;
        }

        VulkanContext& ctx = VulkanContext::current();
        VkDevice device = ctx.device();

        ctx.waitIdle();

        if (m_sampler != VK_NULL_HANDLE) vkDestroySampler(device, m_sampler, nullptr);
        if (m_view != VK_NULL_HANDLE) vkDestroyImageView(device, m_view, nullptr);
        if (m_image != VK_NULL_HANDLE) vkDestroyImage(device, m_image, nullptr);
        if (m_memory != VK_NULL_HANDLE) vkFreeMemory(device, m_memory, nullptr);

        m_sampler = VK_NULL_HANDLE;
        m_view = VK_NULL_HANDLE;
        m_image = VK_NULL_HANDLE;
        m_memory = VK_NULL_HANDLE;
        m_vkFormat = VK_FORMAT_UNDEFINED;
        m_state = {};
        m_mipLevels = 1;
        m_info = {};
    }

    void VulkanTextureImpl::update(const TextureAsset& asset)
    {
        setPixels(asset.pixels);
    }

    void VulkanTextureImpl::setPixels(const vector<Color>& pixels)
    {
        AIKO_ASSERT(isValid(), "Invalid texture");
        AIKO_ASSERT(m_info.type == TextureType::Sampled, "setPixels only supports sampled textures for now");
        AIKO_ASSERT(m_info.format == TextureFormat::RGBA8 || m_info.format == TextureFormat::BGRA8, "Unsupported sampled texture format");

        const uint32_t pixelCount = m_info.width * m_info.height;
        AIKO_ASSERT(pixels.size() == pixelCount, "Mismatch pixels count and texture size");

        std::vector<uint8_t> packed(pixelCount * 4);

        auto toByte = [](float v) -> uint8_t
        {
            return static_cast<uint8_t>(std::clamp(v, 0.0f, 1.0f) * 255.0f);
        };

        const bool bgra = m_info.format == TextureFormat::BGRA8;

        for (uint32_t i = 0; i < pixelCount; ++i)
        {
            const Color& c = pixels[i];

            const uint8_t r = toByte(c.r);
            const uint8_t g = toByte(c.g);
            const uint8_t b = toByte(c.b);
            const uint8_t a = toByte(c.a);

            packed[i * 4 + 0] = bgra ? b : r;
            packed[i * 4 + 1] = g;
            packed[i * 4 + 2] = bgra ? r : b;
            packed[i * 4 + 3] = a;
        }

        VulkanContext& ctx = VulkanContext::current();

        VkBuffer stagingBuffer = VK_NULL_HANDLE;
        VkDeviceMemory stagingMemory = VK_NULL_HANDLE;

        const VkDeviceSize imageSize = packed.size();

        ctx.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingMemory);

        void* data = nullptr;

        const VkResult mapResult = vkMapMemory(ctx.device(), stagingMemory, 0, imageSize, 0, &data);
        AIKO_ASSERT(mapResult == VK_SUCCESS, "Failed to map texture staging memory");

        std::memcpy(data, packed.data(), packed.size());

        vkUnmapMemory(ctx.device(), stagingMemory);

        // ---------------------------------------------------------
        // Record the entire texture upload in one command buffer.
        // ---------------------------------------------------------

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
        };

        vkDestroyBuffer(ctx.device(), stagingBuffer, nullptr);
        vkFreeMemory(ctx.device(), stagingMemory, nullptr);
    }

    VkFormat VulkanTextureImpl::convertToVkFormat(const TextureFormat format)
    {
        switch (format)
        {
            case TextureFormat::BGRA8: return VkFormat::VK_FORMAT_B8G8R8A8_UNORM;
            case TextureFormat::RGBA8: return VkFormat::VK_FORMAT_R8G8B8A8_UNORM;
            case TextureFormat::D24S8: return VkFormat::VK_FORMAT_D24_UNORM_S8_UINT;
            case TextureFormat::D32F: return VkFormat::VK_FORMAT_D32_SFLOAT;
            case TextureFormat::INVALID:
            default:
                AIKO_ASSERT(false, "Texture format not supported!");
                return VK_FORMAT_UNDEFINED;
        }
    }


}
