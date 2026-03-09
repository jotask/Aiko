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
        view.camera = findMainCamera(scene);
        view.ambientLight.color = WHITE;
        view.ambientLight.intensity = 0.75f;
        gatherLights(scene, view);
        return view;
    }

    const Camera* SceneViewBuilder::findMainCamera(Scene& scene)
    {
        // 1) Prefer explicit active camera, if set and valid
        if (GameObject* active = scene.getActiveCamera())
        {
            if (active->hasComponent<CameraComponent>())
            {
                AikoPtr<CameraComponent> cmp = active->getComponent<CameraComponent>();
                return &cmp->getCamera();
            }
        }

        // 2) If no explicit active camera, prefer first camera marked as main
        for (GameObject* obj : scene.getObjects())
        {
            if (obj == nullptr)
            {
                continue;
            }
            if (obj->hasComponent<CameraComponent>() == false)
            {
                continue;
            }
            AikoPtr<CameraComponent> cmp = obj->getComponent<CameraComponent>();
            if (cmp->isMain() == true)
            {
                return &cmp->getCamera();
            }
        }

        // 3) Final fallback: first camera found in scene
        for (GameObject* obj : scene.getObjects())
        {
            if (obj == nullptr)
            {
                continue;
            }
            if (obj->hasComponent<CameraComponent>() == false)
            {
                continue;
            }
            AikoPtr<CameraComponent> cmp = obj->getComponent<CameraComponent>();
            return &cmp->getCamera();
        }

        return nullptr;
    }

    void SceneViewBuilder::gatherLights(Scene& scene, SceneView& out)
    {
        for (GameObject* obj : scene.getObjects())
        {
            if (obj == nullptr)
            {
                continue;
            }

            if (obj->hasComponent<LightComponent>() == false)
            {
                continue;
            }

            AikoPtr<LightComponent> cmp = obj->getComponent<LightComponent>();

            const LightData ld =
            {
                .type = cmp->type,
                .position = obj->transform().position,
                .direction = cmp->direction,
                .color = cmp->color,
                .intensity = cmp->intensity,
                .range = cmp->range,
                .innerCos = cmp->innerCos,
                .outerCos = cmp->outerCos,
            };
            out.lights.push_back(ld);

        }
    }

}
