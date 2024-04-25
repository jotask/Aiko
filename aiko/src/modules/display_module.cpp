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
        m_displayName = "Aiko";
        GLFWwindow* window = glfwCreateWindow(size.x, size.y, m_displayName.c_str(), NULL, NULL);
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

        static float lastTime;
        static long nbFrames;

        // Measure speed
        double currentTime = glfwGetTime();
        double delta = currentTime - lastTime;
        nbFrames++;
        if (delta >= 0.15)
        {

            // If last cout was more than 1 sec ago
            std::cout << 1000.0 / double(nbFrames) << std::endl;

            double fps = double(nbFrames) / delta;

            std::stringstream ss;
            ss << m_displayName.c_str() << " [" << fps << " FPS]";

            glfwSetWindowTitle((GLFWwindow*)m_curent.native, ss.str().c_str());

            nbFrames = 0;
            lastTime = currentTime;
        }

        glfwPollEvents();
        if ( glfwWindowShouldClose((GLFWwindow*) m_curent.native) == true )
        {
            WindowCloseEvent even;
            aiko::EventSystem::it().sendEvent(even);
        }
    }

}
