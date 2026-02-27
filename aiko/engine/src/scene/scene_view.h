#pragma once

#include "models/light.h"

#include <vector>

namespace aiko
{
    class Camera;
    struct SceneView
    {
        const Camera* camera = nullptr;
        AmbientLight ambientLight = {};
        std::vector<LightData> lights;

        void clear()
        {
            camera = nullptr;
            lights.clear();
        }
    };
}
