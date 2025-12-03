#include "time.h"

#include <chrono>

namespace aiko
{
    void Time::update()
    {
        using clock = std::chrono::steady_clock;
        static auto startTime = clock::now();

        // Current time since start in seconds
        auto now = clock::now();
        double current = std::chrono::duration<double>(now - startTime).count();
        time = current;

        // Delta time
        static double lastFrame = current;
        double deltaTime = current - lastFrame;
        lastFrame = current;

        deltaTime = deltaTime;
        frames++;

        // FPS counter
        static double previousTime = current;
        static int frameCount = 0;

        frameCount++;

        if (current - previousTime >= 1.0)
        {
            fps = frameCount;
            frameCount = 0;
            previousTime = current;
        }
    }
}
