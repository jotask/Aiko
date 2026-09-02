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
        if (GameObject* active = scene.getActiveCamera())
        {
            if (CameraComponent* component = active->getComponent<CameraComponent>())
            {
                return &component->getCamera();
            }
        }

        const auto cameras = scene.components<CameraComponent>();

        for (CameraComponent* component : cameras)
        {
            if (component != nullptr && component->isMain())
            {
                return &component->getCamera();
            }
        }

        if (cameras.empty())
        {
            return nullptr;
        }

        return &cameras.front()->getCamera();
    }

    void SceneViewBuilder::gatherLights(Scene& scene, SceneView& out)
    {
        for (LightComponent* component : scene.components<LightComponent>())
        {
            if (component == nullptr)
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
