#pragma once

#include "layers/layer.h"

#include <vector>

namespace aiko
{
    class GameObject;
}

namespace aiko::perf
{
    class PerformanceSmoke final : public Layer
    {
    public:
        PerformanceSmoke() = default;
        ~PerformanceSmoke() override = default;

    protected:
        void init() override;
        void update() override;

    private:
        void initCamera();
        void initWorld();
        void initLights();

        void updateAnimatedObjects();
        void updateStats();

    private:
        std::vector<GameObject*> m_animatedObjects;

        float m_statsTimer = 0.0f;
    };
}
