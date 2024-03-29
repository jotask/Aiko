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
        GLFWwindow* window = glfwCreateWindow(800, 600, "Aiko", NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            // Throw exception and/or exit
            return;
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(0);

        auto lamba = [](GLFWwindow* window, int width, int height)
        {
            // m_curent.m_size = {width, height};
            WindowResizeEvent even(width, height);
            aiko::EventSystem::it().sendEvent(even);
        };

        glfwSetFramebufferSizeCallback(window, lamba );
        m_curent.native = window;

    }

    void* DisplayModule::getNativeDisplay()
    {
        return m_curent.native;
    }

    void DisplayModule::endFrame()
    {
        glfwPollEvents();
        glfwSwapBuffers((GLFWwindow*)m_curent.native);
    }

    void DisplayModule::preUpdate()
    {
        if ( glfwWindowShouldClose((GLFWwindow*) m_curent.native) == true )
        {
            WindowCloseEvent even;
            aiko::EventSystem::it().sendEvent(even);
        }
    }

}
