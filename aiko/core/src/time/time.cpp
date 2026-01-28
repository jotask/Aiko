#include "time.h"

#include <chrono>

namespace aiko
{
    void Time::update()
    {
        using clock = std::chrono::steady_clock;
        static auto startTime = clock::now();

        // Current time since start in seconds
        const auto now = clock::now();
        const double current = std::chrono::duration<double>(now - startTime).count();
        time = current;

        // Delta time
        static double lastFrame = current;
        const double dt = current - lastFrame;
        lastFrame = current;

        deltaTime = dt;
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
