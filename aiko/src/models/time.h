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

    private:
        float deltaTime = 0.0f;
        float fps = 0.0f;
        unsigned int frames = 0;
    };

}
