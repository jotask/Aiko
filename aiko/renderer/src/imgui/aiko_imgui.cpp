#include "aiko_imgui.h"

#include "aiko_renderer.h"
#include "display/display_manager.h"
#include "intrumentor/profiler.h"
#include "platform/bgfx/bgfx_types.h"
#include "time/time.h"

#if defined(AIKO_BGFX)
    #include <bgfx/bgfx.h>
    #include "platform/bgfx/imgui/imgui_impl_bgfx.h"
#elif defined(AIKO_NATIVE)
    #error TODO Not Implemented
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
    AikoImgui::AikoImgui()
        : m_isInitialized(false)
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

        ImGui_ImplGlfw_InitForOther(window, true);
        ImGui_Implbgfx_Init(m_viewId);

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

        ImGui_Implbgfx_NewFrame();
        ImGui::NewFrame();
    }

    void AikoImgui::endFrame(int width, int height)
    {
        AIKO_FUNCTION_PROFILE

        AIKO_ASSERT(m_isInitialized == true, "Calling endFrame without initialize first.");

        ImGui::Render();

        ::bgfx::setViewFrameBuffer(m_viewId, AIKO_INVALID_HANDLE);
        ::bgfx::setViewRect(m_viewId, 0, 0, static_cast<uint16_t>(width), static_cast<uint16_t>(height));
        ::bgfx::touch(m_viewId);

        ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());
    }

}
