#include "aiko_imgui.h"

#include "platform/bgfx/bgfx_types.h"

#if defined(AIKO_BGFX)
    #include <bgfx/bgfx.h>
    #include "platform/bgfx/imgui/imgui_impl_bgfx.h"
#elif defined(AIKO_NATIVE)
    #error TODO Not Implemented
#else
    #error NOT IMPLEMENTED
#endif

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <GLFW/glfw3.h>

#include <logger/logger.h>
#include <aiko_types.h>

namespace aiko
{
    AikoImgui::AikoImgui()
        : m_isInitialized(false)
    {
    }

    void AikoImgui::init(ViewId id, void* w)
    {

        AIKO_ASSERT(m_isInitialized == false, "Imgui already initialized");

        GLFWwindow* window = static_cast<GLFWwindow*>(w);
        AIKO_ASSERT(window != nullptr, "Invalid GLFW window or not supported");

        m_viewId = id;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        ImGui_ImplGlfw_InitForOther(window, true);
        ImGui_Implbgfx_Init(m_viewId);

        logger::Log::info() << "ImGui Version : " << IMGUI_VERSION << " (" << IMGUI_VERSION_NUM << ")";

    }

    void AikoImgui::beginFrame()
    {
        ImGui_Implbgfx_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void AikoImgui::endFrame(int width, int height)
    {
        ImGui::Render();

        ::bgfx::setViewFrameBuffer(m_viewId, AIKO_INVALID_HANDLE);
        ::bgfx::setViewRect(m_viewId, 0, 0, static_cast<uint16_t>(width), static_cast<uint16_t>(height));

        ::bgfx::touch(m_viewId);

        ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());
    }

}
