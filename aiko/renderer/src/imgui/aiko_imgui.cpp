#include "aiko_imgui.h"

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

    void AikoImgui::init(GLFWwindow* window)
    {

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        ::bgfx::ViewId m_kView = 0;

        ImGui_ImplGlfw_InitForOther(window, true);
        ImGui_Implbgfx_Init(m_kView);

        logger::Log::info() << "ImGui Version : " << IMGUI_VERSION << " (" << IMGUI_VERSION_NUM << ")";

    }

    void AikoImgui::beginFrame()
    {
        ImGui_Implbgfx_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void AikoImgui::endFrame()
    {
        // FIXME
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        ImGui::Render();
        ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());
    }

}
