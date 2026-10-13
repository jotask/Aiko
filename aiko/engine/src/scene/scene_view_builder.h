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
        void gatherLights(Scene& scene, SceneView& out);
    };

}