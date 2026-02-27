#pragma once

#include <aiko_types.h>

#include "scene/scene_render_world.h"

namespace aiko
{

    class Scene;
    class GameObject;

    class SceneRenderer
    {
    public:
        SceneRenderer() = default;
        ~SceneRenderer() = default;

        void render(Scene& scene);

    private:

        void submitGameObject(GameObject* obj);
        void submitBatches();

        SceneRenderWorld m_worldRenderer;

    };

}