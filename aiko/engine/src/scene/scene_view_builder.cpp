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
        view.ambientLight.intensity = 0.1f;
        gatherLights(scene, view);
        return view;
    }

    const Camera* SceneViewBuilder::findMainCamera(Scene& scene)
    {
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

            AIKO_NOT_IMPLEMENTED;

            /*
            LightData ld{};
            ld.type = light->type();                  // adjust
            ld.position = go->transform().position;   // adjust; or from transform matrix
            ld.direction = light->direction();        // adjust
            ld.color = light->color();                // adjust
            ld.intensity = light->intensity();        // adjust
            ld.range = light->range();                // adjust
            ld.spotInner = light->innerCos();         // adjust if you store cos values
            ld.spotOuter = light->outerCos();         // adjust
            out.lights.push_back(ld);
            */

        }
    }

}
