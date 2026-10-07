#include "scene_picking.h"

#include "core/editor_context.h"
#include "scene/scene_bounds.h"

#include "models/game_object.h"
#include "scene/scene.h"
#include "systems/scene_system.h"

#include <limits>

namespace aiko::editor
{

    ScenePickResult pickScene(EditorContext& context, const Ray& ray)
    {
        Scene& scene = context.sceneSystem().getScene();

        ScenePickResult result;

        float closestDistance = std::numeric_limits<float>::max();

        for (GameObject* object : scene.getObjects())
        {
            if (object == nullptr || !object->isActiveInHierarchy())
            {
                continue;
            }

            Bounds bounds;

            if (!calculateSceneObjectBounds(context, *object, bounds))
            {
                continue;
            }

            float distance = 0.0f;

            if (!math::intersectRayBounds(ray, bounds, distance))
            {
                continue;
            }

            if (distance >= closestDistance)
            {
                continue;
            }

            closestDistance = distance;

            result.object = object;
            result.distance = distance;
        }

        return result;
    }

}
