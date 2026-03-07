#pragma once

#include <aiko_types.h>

#include "scene/scene_render_world.h"

namespace aiko
{

    class Scene;
    class GameObject;
    class RenderModule;

    class SceneRenderer
    {
    public:
        SceneRenderer();
        ~SceneRenderer() = default;

        void render(RenderModule*, Scene& scene);

    private:

        void submitGameObject(GameObject* obj);
        void submitBatches();

        RenderModule* m_renderModule;
        SceneRenderWorld m_worldRenderer;

    };

}