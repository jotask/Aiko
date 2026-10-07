#include "scene_view_builder.h"

#include "scene.h"
#include "models/game_object.h"
#include "components/camera_component.h"
#include "components/light_component.h"

namespace aiko
{
    SceneView SceneViewBuilder::build(Scene& scene)
    {
        SceneView view;
        view.clear();
        view.camera = findActiveCamera(scene);
        view.clearColor = scene.clearColor();
        view.ambientLight = scene.ambientLight();
        gatherLights(scene, view);
        return view;
    }

    const Camera* SceneViewBuilder::findActiveCamera(Scene& scene)
    {
        if (GameObject* active = scene.getActiveCamera())
        {
            if (active->isActiveInHierarchy())
            {
                CameraComponent* component =
                    active->getComponent<CameraComponent>();

                AIKO_ASSERT(
                    component != nullptr,
                    "Active camera GameObject has no CameraComponent");

                if (component != nullptr &&
                    component->isEnabled())
                {
                    return &component->getCamera();
                }
            }
        }

        for (CameraComponent* component :
             scene.components<CameraComponent>())
        {
            if (component == nullptr ||
                component->isActiveAndEnabled() == false)
            {
                continue;
            }

            return &component->getCamera();
        }

        return nullptr;
    }

    void SceneViewBuilder::gatherLights(Scene& scene, SceneView& out)
    {
        for (LightComponent* component : scene.components<LightComponent>())
        {
            if (component == nullptr || component->isActiveAndEnabled() == false)
            {
                continue;
            }

            GameObject* object = component->getGameObject();
            AIKO_ASSERT(object != nullptr, "LightComponent is not attached to a GameObject");

            const LightData light =
            {
                .type = component->type,
                .position = object->transform().position,
                .direction = component->direction,
                .color = component->color,
                .intensity = component->intensity,
                .range = component->range,
                .innerCos = component->innerCos,
                .outerCos = component->outerCos,
            };

            out.lights.push_back(light);
        }
    }

}
