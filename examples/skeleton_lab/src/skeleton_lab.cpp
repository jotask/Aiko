#include "skeleton_lab.h"

#include "layers/contexts/asset_context.h"
#include "layers/contexts/render_context.h"
#include "layers/contexts/scene_context.h"
#include "models/mesh_factory.h"

#include <components/camera_component.h>
#include <components/mesh_component.h>

#include <cmath>

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

    void SkeletonLab::update()
    {
        if (!m_model.isValid())
        {
            return;
        }

        m_animationTime += getDeltaTime();
        const float angle = std::sin(m_animationTime * 2.0f) * 0.6f;

        {
            const bool updated = m_model.setJointRotation("LeftArm", angle, {0.0f, 0.0f, 1.0f});
            AIKO_ASSERT(updated, "LeftArm joint not found");
        }

        {
            const bool updated = m_model.setJointRotation("RightArm", angle, {0.0f, 0.0f, 1.0f});
            AIKO_ASSERT(updated, "RightArm joint not found");
        }
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

