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
            .RenderPass = ctx.clearRenderPass(),
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
        m_samplerCache = std::make_unique<VulkanSamplerCache>(ctx);
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

        if (m_samplerCache != nullptr)
        {
            m_samplerCache->destroy();
            m_samplerCache.reset();
        }

        ImGui_ImplGlfw_Shutdown();
    }

    ImguiTextureId VulkanImguiImpl::textureId(const interfaces::ITextureImpl& texture, const SamplerState& sampler)
    {
        const auto& vulkanTexture = static_cast<const VulkanTextureImpl&>(texture);

        AIKO_ASSERT(vulkanTexture.isValid(), "Cannot register invalid Vulkan ImGui texture");
        AIKO_ASSERT(m_samplerCache != nullptr, "Vulkan ImGui sampler cache is not initialized");

        const VkImageView imageView = vulkanTexture.imageView();

        const TextureBindingKey key
        {
            .texture = &texture,
            .sampler = sampler,
        };

        TextureBinding& binding = m_textureBindings[key];

        if (binding.descriptorSet == VK_NULL_HANDLE || binding.imageView != imageView)
        {
            if (binding.descriptorSet != VK_NULL_HANDLE)
            {
                ImGui_ImplVulkan_RemoveTexture(binding.descriptorSet);
                binding.descriptorSet = VK_NULL_HANDLE;
            }

            binding.imageView = imageView;

            const VkSampler vkSampler = m_samplerCache->getOrCreate(sampler);

            binding.descriptorSet = ImGui_ImplVulkan_AddTexture(vkSampler, imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

            AIKO_ASSERT(binding.descriptorSet != VK_NULL_HANDLE, "Failed to register Vulkan ImGui texture");
        }

        return static_cast<ImguiTextureId>(reinterpret_cast<uintptr_t>( binding.descriptorSet ));
    }
}
