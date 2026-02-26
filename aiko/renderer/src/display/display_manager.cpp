#include "display_manager.h"

#include "display/display_events.hpp"

#include <logger/logger.h>
#include <aiko_types.h>
#include <events/events.hpp>

#include "input/inputs_types.h"
#include "time/time.h"

namespace aiko
{

    void DisplayManager::init(string title, const uint width, uint height)
    {

        AIKO_ASSERT(glfwInit() == GLFW_TRUE, "Failed to init Window")

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // no OpenGL
        GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
        if (window == NULL)
        {
            logger::Log::critical("Failed to create GLFW window");
            glfwTerminate();
            AIKO_ASSERT(false, "Failed to create GLFW window")
        }

        auto glfwSetWindowCenter = [](GLFWwindow* window) -> bool
        {
            
            if (window == nullptr)
            {
                return false;
            }

            int windowWidth, windowHeight;
            glfwGetWindowSize(window, &windowWidth, &windowHeight);

            int monitorCount;
            GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
            if (!monitors || monitorCount == 0)
            {
                return false;
            }

            // Pick the monitor with the biggest intersection with the window
            GLFWmonitor* bestMonitor = monitors[0];
            int bestArea = 0;

            int wx, wy;
            glfwGetWindowPos(window, &wx, &wy);

            for (int i = 0; i < monitorCount; ++i)
            {
                int mx, my;
                glfwGetMonitorPos(monitors[i], &mx, &my);
                const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);
                if (!mode) continue;

                const int overlapX = std::max(0, std::min(wx + windowWidth, mx + mode->width) - std::max(wx, mx));
                const int overlapY = std::max(0, std::min(wy + windowHeight, my + mode->height) - std::max(wy, my));
                const int area = overlapX * overlapY;

                if (area > bestArea)
                {
                    bestArea = area;
                    bestMonitor = monitors[i];
                }
            }

            // Center on the best monitor
            const GLFWvidmode* mode = glfwGetVideoMode(bestMonitor);
            if (mode == nullptr)
            {
                return false;
            }

            int mx, my;
            glfwGetMonitorPos(bestMonitor, &mx, &my);

            int posX = mx + (mode->width - windowWidth) / 2;
            int posY = my + (mode->height - windowHeight) / 2;

            glfwSetWindowPos(window, posX, posY);

            return true;
            
        };
        
        glfwSetWindowCenter(window);
        
        glfwMakeContextCurrent(window);
        glfwSwapInterval(0);
        glfwShowWindow(window);
        
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

        // Mouse
        auto mouse_callback = [](GLFWwindow* window, int button, int action, int mods)
        {
            OnMouseKeyPressedEvent even(button, action, mods);
            aiko::EventSystem::it().sendEvent(even);
        };
        glfwSetMouseButtonCallback(window, mouse_callback);

        // Mouse Scrollback
        auto mouse_scroll_callback = [](GLFWwindow* window, double xoffset, double yoffset)
        {
            OnMouseScrollCallbackEvent even(xoffset, yoffset);
            aiko::EventSystem::it().sendEvent(even);
        };
        glfwSetScrollCallback(window, mouse_scroll_callback);

        // Mouse position
        auto cursor_position_callback = [](GLFWwindow* window, double xpos, double ypos)
            {
                OnMouseMoveEvent even(xpos, ypos);
                aiko::EventSystem::it().sendEvent(even);
            };
        glfwSetCursorPosCallback(window, cursor_position_callback);

        int glfw_major, glfw_minor, glfw_patch;
        glfwGetVersion(&glfw_major, &glfw_minor, &glfw_patch);
        logger::Log::info() << "GLFW Version: " << glfw_major << "." << glfw_minor << "." << glfw_patch;

        m_display.setWindowTitle(title);
        m_display.setWindowSize(width, height);

        m_native = window;

        EventSystem::it().bind<OnKeyPressedEvent>(this, &DisplayManager::onKeyPressed);
        EventSystem::it().bind<WindowResizeEvent>(this, &DisplayManager::onWindowResize);

    }

    void DisplayManager::swap()
    {
        glfwSwapBuffers(m_native);
    }

    void DisplayManager::dispose()
    {
        glfwDestroyWindow(m_native);
        glfwTerminate();
    }

    Display* DisplayManager::getDisplay()
    {
        return &m_display;
    }

    GLFWwindow * DisplayManager::getNativeWindow() const
    {
        return m_native;
    }

    void DisplayManager::onKeyPressed(Event& event)
    {
        const auto& msg = static_cast<const OnKeyPressedEvent&>(event);
        const Key key = static_cast<Key>(msg.key);
        glfwSetWindowShouldClose(m_native, key == KEY_ESCAPE);
    }

    void DisplayManager::onWindowResize(Event& event)
    {
        const auto& msg = static_cast<const WindowResizeEvent&>(event);
        m_display.setWindowSize(msg.width, msg.height);
    }

    void DisplayManager::update()
    {
        if ( glfwWindowShouldClose(m_native) == true )
        {
            WindowCloseEvent even;
            EventSystem::it().sendEvent(even);
        }

        string title = m_display.getWindowTitle() + " : [" + std::format("{:.0f}", Time::it().getFps() ) + " FPS]";
        glfwSetWindowTitle(m_native, title.c_str());


    }

}
