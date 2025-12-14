#pragma once

#include "aiko_types.h"
#include "core/singleton.h"

namespace aiko
{
    class AIKO_API Time : public Singleton<Time>
    {
    public:

        float getDeltaTime() const
        {
            return deltaTime;
        }

        double secondSinceStart() const
        {
            return time;
        }

        void update();

    private:
        double time = 0.0f;
        float deltaTime = 0.0f;
        float fps = 0.0f;
        uint frames = 0;
    };

}
