#pragma once

#include <math/math_bounds.h>

namespace aiko
{
    class GameObject;
}

namespace aiko::editor
{

    class EditorContext;

    struct ScenePickResult
    {
        GameObject* object = nullptr;
        float distance = 0.0f;

        explicit operator bool() const { return object != nullptr; }
    };

    ScenePickResult pickScene(EditorContext& context, const Ray& ray);

}
