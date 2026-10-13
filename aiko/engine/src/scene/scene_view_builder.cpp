#include "scene_view_builder.h"

#include "scene.h"
#include "models/game_object.h"
#include "components/light_component.h"

namespace aiko
{
    SceneView SceneViewBuilder::build(Scene& scene)
    {
        SceneView view;
        view.clear();
        view.camera = scene.resolveCamera();
        view.clearColor = scene.clearColor();
        view.ambientLight = scene.ambientLight();
        gatherLights(scene, view);
        return view;
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
