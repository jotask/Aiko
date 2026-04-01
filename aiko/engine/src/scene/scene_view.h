#pragma once

#include "models/light.h"

namespace aiko
{
    class Camera;
    struct SceneView
    {
        const Camera* camera = nullptr;
        AmbientLight ambientLight = {};
        vector<LightData> lights;

        void clear()
        {
            camera = nullptr;
            lights.clear();
        }
    };
}
