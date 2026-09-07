#pragma once

#include "aiko_types.h"
#include "core/singleton.h"

namespace aiko
{
    class Time : public Singleton<Time>
    {
    public:

        float getDeltaTime() const
        {
            return deltaTime;
        }

        float getFps() const
        {
            return fps;
        }

        float getAverageFps() const
        {
            return averageFps;
        }

        double secondSinceStart() const
        {
            return time;
        }

        void update();

    private:

        static constexpr size_t FpsAverageSamples = 10;
        static inline std::array<double, FpsAverageSamples> fpsSamples{};
        static inline size_t fpsSampleIndex = 0;
        static inline size_t fpsSampleCount = 0;
        static inline double fpsSampleSum = 0.0;
        static inline double averageFps = 0.0;

        double time = 0.0f;
        float deltaTime = 0.0f;
        float fps = 0.0f;
        uint frames = 0;
    };

}
