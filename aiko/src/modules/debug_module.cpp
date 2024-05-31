#include "modules/debug_module.h"

#include "models/time.h"

#include "aiko_types.h"
#include "core/libs.h"
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
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init();

        EventSystem::it().bind<WindowResizeEvent>(this, &DebugModule::onKeyPressed);
    }

    void DebugModule::beginFrame()
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void DebugModule::endFrame()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void DebugModule::onKeyPressed(Event& event)
    {
        const auto& msg = static_cast<const WindowResizeEvent&>(event);
        int a = 0;
    }

}
