#pragma once

#include "scene/scene_view.h"

namespace aiko
{

    class Scene;

    class SceneViewBuilder
    {
    public:
        SceneViewBuilder() = default;
        SceneView build(Scene& scene);
    private:
        const Camera* findActiveCamera(Scene& scene);
        void gatherLights(Scene& scene, SceneView& out);
    };

}