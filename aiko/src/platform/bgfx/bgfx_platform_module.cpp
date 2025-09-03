#ifdef AIKO_BGFX

#include "modules/platform_module.h"

#include <memory>

#include "modules/render/render_module.h"
#include "modules/module_connector.h"

#include "core/libs.h"
#include "models/time.h"
#include <GLFW/glfw3.h>

namespace aiko
{

    const char* PlatformModule::loadTextContent(char* filePath)
    {
        return nullptr;
    }

    void PlatformModule::preUpdate()
    {
        updateTime();
    }

    void PlatformModule::updateTime()
    {
        auto& time = Time::it();

        // Delta Time
        time.time = glfwGetTime();
        static double lastFrame = time.time;
        static double deltaTime;

        // Fps
        static double previousTime = glfwGetTime();
        static int frameCount = 0;

        deltaTime = time.time - lastFrame;
        lastFrame = time.time;

        time.deltaTime = deltaTime;
        time.frames++;

        frameCount++;
        // If a second has passed.
        if (time.time - previousTime >= 1.0)
        {
            // Display the frame count here any way you want.
            time.fps = frameCount;

            frameCount = 0;
            previousTime = time.time;
        }

    }

}

#endif