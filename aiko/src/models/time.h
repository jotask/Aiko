#pragma once

#include "shared/singleton.h"

namespace aiko
{
    class Time : public Singleton<Time>
    {
        friend class PlatformModule;
        friend class Aiko;
    public:

        float getDeltaTime() const
        {
            return deltaTime;
        }

        double secondSinceStart() const
        {
            return time;
        }

    private:
        double time = 0.0f;
        float deltaTime = 0.0f;
        float fps = 0.0f;
        uint frames = 0;
    };

}
