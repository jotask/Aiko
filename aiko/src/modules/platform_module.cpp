#include "modules/platform_module.h"

#include <memory>

#include "modules/render_module.h"
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
        // Delta Time
        static double currentFrame = glfwGetTime();
        static double lastFrame = currentFrame;
        static double deltaTime;

        // Fps
        static double previousTime = glfwGetTime();
        static int frameCount = 0;

        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        auto& time = Time::it();
        time.deltaTime = deltaTime;
        time.frames++;

        frameCount++;
        // If a second has passed.
        if (currentFrame - previousTime >= 1.0)
        {
            // Display the frame count here any way you want.
            time.fps = frameCount;

            frameCount = 0;
            previousTime = currentFrame;
        }

    }

}
