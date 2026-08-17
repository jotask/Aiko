#include "aiko_imgui.h"

#include "aiko_renderer.h"
#include "display/display_manager.h"
#include "intrumentor/profiler.h"
#include "time/time.h"

#if defined(AIKO_BGFX)
    #include "platform/bgfx/imgui/imgui_impl_bgfx.h"
#elif defined(AIKO_NATIVE)
    #error TODO Not Implemented
#elif defined(AIKO_VULKAN)
    #include "platform/vulkan/imgui/vulkan_imgui_aiko.h"
#else
    #error NOT IMPLEMENTED
#endif

#include <imgui_impl_glfw.h>
#include <imgui.h>
#include <GLFW/glfw3.h>

#include <logger/logger.h>
#include <aiko_types.h>

namespace aiko
{

    namespace
    {
        AikoPtr<AikoImguiImpl> getAikoImguiImpl()
        {
            #if defined (AIKO_BGFX)
            return std::make_unique<renderer::bgfx::BgfxImguiImpl>();
            #elif defined (AIKO_NATIVE)
            #error Not implemented
            #elif defined (AIKO_VULKAN)
            return std::make_unique<renderer::vulkan::VulkanImguiImpl>();
            #else
            #error Backend not supported
            #endif
        }
    }

    AikoImgui::AikoImgui()
        : m_isInitialized(false)
        , backend(std::move(getAikoImguiImpl()))
    {
    }

    void AikoImgui::init(const ViewId id, GLFWwindow* window)
    {

        logger::Log::info("ImGui v.%s", IMGUI_VERSION);

        AIKO_ASSERT(m_isInitialized == false, "Imgui already initialized");
        AIKO_ASSERT(window != nullptr, "Invalid GLFW window or not supported");

        m_viewId = id;
        m_window = window;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        io.BackendPlatformName = "AikoImgui";

        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        backend->init(id, window);

        m_isInitialized = true;

        logger::Log::info() << "ImGui Version : " << IMGUI_VERSION << " (" << IMGUI_VERSION_NUM << ")";

    }

    void AikoImgui::beginFrame(int width, int height)
    {
        AIKO_FUNCTION_PROFILE

        AIKO_ASSERT(m_isInitialized == true, "Calling beingFrame without initialize first.");

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
        io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
        io.DeltaTime = Time::it().getDeltaTime();

        backend->beginFrame(width, height);

        ImGui::NewFrame();

    }

    void AikoImgui::endFrame(int width, int height)
    {
        AIKO_FUNCTION_PROFILE
        AIKO_ASSERT(m_isInitialized == true, "Calling endFrame without initialize first.");
        ImGui::Render();
        backend->endFrame(width, height);
    }

}
