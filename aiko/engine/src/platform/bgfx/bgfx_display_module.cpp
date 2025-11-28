#ifdef AIKO_BGFX

#include "bgfx_display_module.h"

#include "aiko.h"
#include "core/libs.h"
#include "core/log.h"
#include "aiko_types.h"
#include "math/math_transform.h"

#include "events/events.hpp"

#include <GLFW/glfw3.h>

namespace aiko::bgfx
{

    GLFWwindow* getNative(void* ptr)
    {
        return (GLFWwindow*)ptr;
    }

    BgfxDisplayModule::~BgfxDisplayModule()
    {

    }

    void BgfxDisplayModule::preInit()
    {

        DisplayModule::init();

        if (glfwInit() == GLFW_FALSE) return exitWithCode(EXIT_CODE::GLFW_INIT);

        const AikoConfig cfg = getAiko()->getConfig();
        const ivec2 size = { cfg.width, cfg.height };
        m_displayName = cfg.window_tittle;
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // no OpenGL
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
            
            if (window == nullptr)
                return false;

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

                int overlapX = std::max(0, std::min(wx + windowWidth, mx + mode->width) - std::max(wx, mx));
                int overlapY = std::max(0, std::min(wy + windowHeight, my + mode->height) - std::max(wy, my));
                int area = overlapX * overlapY;

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

        m_curent.setNative(window);
        m_curent.setWindowSize(size.x, size.y);

        int glfw_major, glfw_minor, glfw_patch;
        glfwGetVersion(&glfw_major, &glfw_minor, &glfw_patch);
        Log::info() << "GLFW Version: " << glfw_major << "." << glfw_minor << "." << glfw_patch;

    }

    void BgfxDisplayModule::beginFrame()
    {
    }

    void BgfxDisplayModule::endFrame()
    {
        glfwSwapBuffers(getNative(m_curent.getNative()));
    }

    void BgfxDisplayModule::dispose()
    {
        glfwDestroyWindow(getNative(m_curent.getNative()));
        glfwTerminate();
    }

    void BgfxDisplayModule::preUpdate()
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