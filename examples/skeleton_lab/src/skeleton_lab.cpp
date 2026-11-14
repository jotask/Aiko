#include "skeleton_lab.h"

#include "layers/contexts/asset_context.h"
#include "layers/contexts/render_context.h"
#include "layers/contexts/scene_context.h"
#include "models/mesh_factory.h"

#include <components/camera_component.h>
#include <components/mesh_component.h>

namespace lab
{

    void SkeletonLab::init()
    {

        scene().ambientLight().intensity = 0.25f;

        aiko::CameraComponent* camera = scene().createCamera(aiko::camera::CameraController::Fly, aiko::Camera::CameraType::Perspective);
        camera->getCamera().position = { 0.0f, 1.0f, 2.0f };
        camera->getCamera().target = { 0.0f, 0.75f, 0.0f };

        m_model = assets().loadModel("project://Rumba_Dancing.fbx");

    }

    void SkeletonLab::render()
    {
        if (m_model.isValid() == true)
        {
            aiko::Transform transform;
            transform.scale = {0.0075f};
            renderer().drawModel(transform, m_model);
        }
    }

}

