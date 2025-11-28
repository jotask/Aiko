#ifdef AIKO_BGFX

#include "modules/debug_module.h"

#include <bgfx/bgfx.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include "imgui/imgui_impl_bgfx.h"

#include "models/time.h"
#include "aiko_types.h"
#include "core/log.h"
#include "modules/module_connector.h"
#include "modules/display_module.h"
#include "events/events.hpp"

namespace aiko
{

    void DebugModule::connect(ModuleConnector* moduleConnector)
    {
        BIND_MODULE_REQUIRED(DisplayModule, moduleConnector, m_displayModule)
    }

    void DebugModule::init()
    {

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        GLFWwindow* window = (GLFWwindow*)m_displayModule->getNativeDisplay();

        ::bgfx::ViewId m_kView = 0;

        ImGui_ImplGlfw_InitForOther(window, true);
        ImGui_Implbgfx_Init(m_kView);

        EventSystem::it().bind<WindowResizeEvent>(this, &DebugModule::onKeyPressed);

        Log::info() << "ImGui Version : " << IMGUI_VERSION << " (" << IMGUI_VERSION_NUM << ")";

    }

    void DebugModule::beginFrame()
    {
        // Start the Dear ImGui frame
        ImGui_Implbgfx_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void DebugModule::endFrame()
    {
        // FIXME
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        ImGui::Render();
        ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());
    }

    void DebugModule::onKeyPressed(Event& event)
    {
        const auto& msg = static_cast<const WindowResizeEvent&>(event);
        int a = 0;
    }

}

#endif