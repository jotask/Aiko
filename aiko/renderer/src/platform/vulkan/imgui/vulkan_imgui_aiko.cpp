#include "vulkan_imgui_aiko.h"

#include "display/display_manager.h"
#include "platform/vulkan/vulkan_context.h"
#include "platform/vulkan/vulkan_platform_helper.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include "platform/vulkan/impl/vulkan_texture_impl.h"

namespace aiko::renderer::vulkan
{

    void VulkanImguiImpl::init(const ViewId id, GLFWwindow* window)
    {

        ImGui_ImplGlfw_InitForVulkan(window, true);

        VulkanContext& ctx = VulkanContext::current();

        QueueFamilyIndices indices = ctx.findQueueFamilies(ctx.physicalDevice());

        const ImGui_ImplVulkan_PipelineInfo pipelineInfo =
        {
            .RenderPass = ctx.renderPass(),
            .Subpass = 0,
            .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
        };

        ImGui_ImplVulkan_InitInfo initInfo =
        {
            .ApiVersion = VK_API_VERSION_1_0,
            .Instance = ctx.instance(),
            .PhysicalDevice = ctx.physicalDevice(),
            .Device = ctx.device(),
            .QueueFamily = indices.graphicsFamily.value(),
            .Queue = ctx.graphicsQueue(),
            .DescriptorPool = VK_NULL_HANDLE,
            .DescriptorPoolSize = 1000,
            .MinImageCount = static_cast<uint32_t>(ctx.swapChainImages().size()),
            .ImageCount = static_cast<uint32_t>(ctx.swapChainImages().size()),
            .PipelineCache = VK_NULL_HANDLE,
            .PipelineInfoMain = pipelineInfo,
            .UseDynamicRendering = false,
            .Allocator = nullptr,
            .CheckVkResultFn = checkImGuiVkResult,
        };

        if (ImGui_ImplVulkan_Init(&initInfo) == false)
        {
            logger::Log::error("Failed to initialize ImGui Vulkan backend!");
        }

        const VkSamplerCreateInfo samplerInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .mipLodBias = 0.0f,
            .anisotropyEnable = VK_FALSE,
            .maxAnisotropy = 1.0f,
            .compareEnable = VK_FALSE,
            .compareOp = VK_COMPARE_OP_ALWAYS,
            .minLod = 0.0f,
            .maxLod = 0.0f,
            .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
            .unnormalizedCoordinates = VK_FALSE,
        };

        const VkResult samplerResult = vkCreateSampler(ctx.device(), &samplerInfo, nullptr, &m_textureSampler);
        AIKO_ASSERT(samplerResult == VK_SUCCESS, "Failed to create Vulkan ImGui texture sampler");

    }

    void VulkanImguiImpl::beginFrame(const ViewId id, int width, int height)
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
    }

    void VulkanImguiImpl::endFrame(const ViewId id, int width, int height)
    {
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), VulkanContext::current().activeCommandBuffer() );
    }

    void VulkanImguiImpl::dispose()
    {

        VulkanContext& ctx = VulkanContext::current();

        ImGui_ImplVulkan_Shutdown();

        m_textureBindings.clear();

        if (m_textureSampler != VK_NULL_HANDLE)
        {
            vkDestroySampler(ctx.device(), m_textureSampler, nullptr);
            m_textureSampler = VK_NULL_HANDLE;
        }

        ImGui_ImplGlfw_Shutdown();
    }

    ImguiTextureId VulkanImguiImpl::textureId(const interfaces::ITextureImpl& texture)
    {
            const auto& vulkanTexture = static_cast<const VulkanTextureImpl&>(texture);

            AIKO_ASSERT(vulkanTexture.isValid(), "Cannot register invalid Vulkan ImGui texture");

            const VkImageView imageView = vulkanTexture.imageView();

            auto& binding = m_textureBindings[&texture];

            if (binding.descriptorSet == VK_NULL_HANDLE || binding.imageView != imageView)
            {
                binding.imageView = imageView;

                binding.descriptorSet = ImGui_ImplVulkan_AddTexture(
                    m_textureSampler,
                    imageView,
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                );

                AIKO_ASSERT(binding.descriptorSet != VK_NULL_HANDLE, "Failed to register Vulkan ImGui texture");
            }

            return static_cast<ImguiTextureId>(reinterpret_cast<uintptr_t>(binding.descriptorSet)
            );

    }
}
