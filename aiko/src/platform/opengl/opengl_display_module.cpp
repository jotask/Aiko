#ifdef AIKO_NATIVE

#include "opengl_display_module.h"

#include "aiko.h"
#include "core/libs.h"
#include "core/log.h"

#include "events/events.hpp"
#include <GLFW/glfw3.h>

namespace aiko::native
{

    GLFWwindow* getNative(void* ptr)
    {
        return (GLFWwindow*)ptr;
    }

    OpenglDisplayModule::~OpenglDisplayModule()
    {

    }

    void OpenglDisplayModule::init()
    {

        DisplayModule::init();

        const AikoConfig cfg = getAiko()->getConfig();
        const ivec2 size = { cfg.width, cfg.height };
        m_displayName = cfg.window_tittle;
        GLFWwindow* window = glfwCreateWindow(size.x, size.y, m_displayName.c_str(), NULL, NULL);
        if (window == NULL)
        {
            Log::critical("Failed to create GLFW window");
            glfwTerminate();
            // Throw exception and/or exit
            return;
        }

        auto glfwSetWindowCenter = [](GLFWwindow* window) -> bool
        {

            if (!window)
                return false;

            int sx = 0, sy = 0;
            int px = 0, py = 0;
            int mx = 0, my = 0;
            int monitor_count = 0;
            int best_area = 0;
            int final_x = 0, final_y = 0;

            glfwGetWindowSize(window, &sx, &sy);
            glfwGetWindowPos(window, &px, &py);

            // Iterate throug all monitors
            GLFWmonitor** m = glfwGetMonitors(&monitor_count);
            if (!m)
                return false;

            for (int j = 0; j < monitor_count; ++j)
            {

                glfwGetMonitorPos(m[j], &mx, &my);
                const GLFWvidmode* mode = glfwGetVideoMode(m[j]);
                if (!mode)
                    continue;

                // Get intersection of two rectangles - screen and window
                int minX = std::max(mx, px);
                int minY = std::max(my, py);

                int maxX = std::min(mx + mode->width, px + sx);
                int maxY = std::min(my + mode->height, py + sy);

                // Calculate area of the intersection
                int area = std::max(maxX - minX, 0) * std::max(maxY - minY, 0);

                // If its bigger than actual (window covers more space on this monitor)
                if (area > best_area)
                {
                    // Calculate proper position in this monitor
                    final_x = mx + (mode->width - sx) / 2;
                    final_y = my + (mode->height - sy) / 2;

                    best_area = area;
                }

            }

            // We found something
            if (best_area)
            {
                glfwSetWindowPos(window, final_x, final_y);
            }

            // Something is wrong - current window has NOT any intersection with any monitors. Move it to the default one.
            else
            {
                GLFWmonitor* primary = glfwGetPrimaryMonitor();
                if (primary)
                {
                    const GLFWvidmode* desktop = glfwGetVideoMode(primary);

                    if (desktop)
                        glfwSetWindowPos(window, (desktop->width - sx) / 2, (desktop->height - sy) / 2);
                    else
                        return false;
                }
                else
                    return false;
            }

            return true;

        };

        glfwSetWindowCenter(window);

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

        m_curent.setNative(window);
        m_curent.setWindowSize(size.x, size.y);

    }

    void OpenglDisplayModule::beginFrame()
    {
    }

    void OpenglDisplayModule::endFrame()
    {
        glfwSwapBuffers(getNative(m_curent.getNative()));
    }

    void OpenglDisplayModule::dispose()
    {
        glfwDestroyWindow(getNative(m_curent.getNative()));
    }

    void OpenglDisplayModule::preUpdate()
    {

        static float lastTime;
        static long nbFrames;

        // Measure speed
        double currentTime = glfwGetTime();
        double delta = currentTime - lastTime;
        nbFrames++;
        if (delta >= 0.15)
        {

            double fps = double(nbFrames) / delta;

            std::stringstream ss;
            ss << m_displayName.c_str() << " [" << fps << " FPS]";

            glfwSetWindowTitle(getNative(m_curent.getNative()), ss.str().c_str());

            nbFrames = 0;
            lastTime = currentTime;
        }

        glfwPollEvents();
        if ( glfwWindowShouldClose(getNative(m_curent.getNative())) == true )
        {
            WindowCloseEvent even;
            aiko::EventSystem::it().sendEvent(even);
        }
    }

}

#endif