#ifdef AIKO_BGFX

#include "bgfx_display_module.h"

#include "aiko.h"
#include "core/libs.h"
#include "core/log.h"

#include "events/events.hpp"

namespace aiko::fx
{

    GLFWwindow* getNative(void* ptr)
    {
        return (GLFWwindow*)ptr;
    }

    BgfxDisplayModule::BgfxDisplayModule(Aiko* aiko)
        : aiko::DisplayModule(aiko)
        , m_window(nullptr)
    {

    }

    BgfxDisplayModule::~BgfxDisplayModule()
    {

    }

    void BgfxDisplayModule::preInit()
    {

        const AikoConfig cfg = getAiko()->getConfig();
        const ivec2 size = { cfg.width, cfg.height };
        m_displayName = cfg.window_tittle;

        if (glfwInit() ==false)
        {
            Log::critical("Failed to GLFW init");
            return;
        }

        m_window = glfwCreateWindow(size.x, size.y, m_displayName.c_str(), NULL, NULL);
        if (m_window == NULL)
        {
            Log::critical("Failed to create GLFW window");
            glfwTerminate();
            // Throw exception and/or exit
            return;
        }
        glfwMakeContextCurrent(m_window);
        glfwSwapInterval(0);

        // window resize
        auto lamba = [](GLFWwindow* window, int width, int height)
        {
            WindowResizeEvent even(width, height);
            aiko::EventSystem::it().sendEvent(even);
        };
        glfwSetFramebufferSizeCallback(m_window, lamba);

        // keyboard
        auto key_callback = [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            OnKeyPressedEvent even(key, scancode, action, mods);
            aiko::EventSystem::it().sendEvent(even);
        };

        glfwSetKeyCallback(m_window, key_callback);

        // Mouse position
        auto cursor_position_callback = [](GLFWwindow* window, double xpos, double ypos)
        {
            OnMouseMoveEvent even(xpos, ypos);
            aiko::EventSystem::it().sendEvent(even);
        };

        glfwSetCursorPosCallback(m_window, cursor_position_callback);
        m_curent.setNative(m_window);
        m_curent.setWindowSize(size.x, size.y);
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