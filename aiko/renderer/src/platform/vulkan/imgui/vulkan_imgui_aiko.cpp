#include "vulkan_imgui_aiko.h"

#include "display/display_manager.h"
#include "platform/vulkan/vulkan_context.h"
#include "platform/vulkan/vulkan_platform_helper.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

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
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
    }
}
