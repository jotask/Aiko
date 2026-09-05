#pragma once

#include "models/camera.h"
#include "renderer/render_queue.h"

namespace aiko
{
    class RenderTarget;

    struct SceneRenderRequest
    {
        Camera camera;
        RenderTarget* target = nullptr;
        RenderQueue queue;
    };
}
