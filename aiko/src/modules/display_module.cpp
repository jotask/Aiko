#include "modules/display_module.h"

#include "events/events.hpp"

#include <GLFW/glfw3.h>

namespace aiko
{

    DisplayModule::~DisplayModule()
    {

    }

    void DisplayModule::init()
    {


        EventSystem::it().bind<WindowResizeEvent>(this, &DisplayModule::onWindowResize);

        // TODO Initial window size from config
        const ivec2 size = { 800, 600 };
        GLFWwindow* window = glfwCreateWindow(size.x, size.y, "Aiko", NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            // Throw exception and/or exit
            return;
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(0);

        // window resize
        auto lamba = [](GLFWwindow* window, int width, int height)
        {
            WindowResizeEvent even(width, height);
            aiko::EventSystem::it().sendEvent(even);
        };
        glfwSetFramebufferSizeCallback(window, lamba );

        // keyboard
        auto key_callback = [](GLFWwindow* window, int key, int scancode, int action, int mods)
            {
                OnKeyPressedEvent even(key, scancode, action, mods);
                aiko::EventSystem::it().sendEvent(even);
            };

        glfwSetKeyCallback(window, key_callback);

        // Mouse position
        auto cursor_position_callback = [](GLFWwindow* window, double xpos, double ypos)
            {
                OnMouseMoveEvent even(xpos, ypos);
                aiko::EventSystem::it().sendEvent(even);
            };

        glfwSetCursorPosCallback(window, cursor_position_callback);
        m_curent.native = window;
        m_curent.m_size = size;

    }

    void DisplayModule::onWindowResize(Event& event)
    {
        const auto& msg = static_cast<const WindowResizeEvent&>(event);
        m_curent.m_size = { msg.width, msg.height };
    }

    void* DisplayModule::getNativeDisplay()
    {
        return m_curent.native;
    }

    void DisplayModule::endFrame()
    {
        glfwSwapBuffers((GLFWwindow*)m_curent.native);
    }

    void DisplayModule::preUpdate()
    {
        glfwPollEvents();
        if ( glfwWindowShouldClose((GLFWwindow*) m_curent.native) == true )
        {
            WindowCloseEvent even;
            aiko::EventSystem::it().sendEvent(even);
        }
    }

}
