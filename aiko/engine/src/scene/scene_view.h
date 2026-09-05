#pragma once

#include "models/light.h"

namespace aiko
{
    class Camera;
    struct SceneView
    {
        const Camera* camera = nullptr;
        Color clearColor = RAYWHITE;
        AmbientLight ambientLight = {};
        vector<LightData> lights;

        void clear()
        {
            camera = nullptr;
            lights.clear();
        }
    };
}
