#include "time.h"

#include <chrono>
#include <intrumentor/profiler.h>

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

            const double elapsed = current - previousTime;
            fps = static_cast<double>(frameCount) / elapsed;

            if (fpsSampleCount < FpsAverageSamples)
            {
                fpsSamples[fpsSampleIndex] = fps;
                fpsSampleSum += fps;
                ++fpsSampleCount;
            }
            else
            {
                fpsSampleSum -= fpsSamples[fpsSampleIndex];
                fpsSamples[fpsSampleIndex] = fps;
                fpsSampleSum += fps;
            }

            fpsSampleIndex = (fpsSampleIndex + 1) % FpsAverageSamples;
            averageFps = fpsSampleSum / static_cast<double>(fpsSampleCount);

            AIKO_PLOT("FPS", fps);
            AIKO_PLOT("Average FPS", averageFps);

            frameCount = 0;
            previousTime = current;
        }
    }
}
