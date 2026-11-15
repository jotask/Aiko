#include "skeleton_lab.h"

#include "layers/contexts/asset_context.h"
#include "layers/contexts/render_context.h"
#include "layers/contexts/scene_context.h"
#include "models/mesh_factory.h"

#include <components/light_component.h>
#include <components/camera_component.h>
#include <components/mesh_component.h>

#include <cmath>

namespace lab
{

    void SkeletonLab::init()
    {

        scene().clearColor() = aiko::BLACK;

        scene().ambientLight().intensity = 0.15f;

        aiko::CameraComponent* camera = scene().createCamera(aiko::camera::CameraController::Fly, aiko::Camera::CameraType::Perspective);
        camera->getCamera().position = {0.0f, 1.5f, 3.5f};
        camera->getCamera().target = {0.0f, 0.35f, 0.0f};

        auto* sun = Instantiate("Sun")->addComponent<aiko::LightComponent>();
        sun->setDirectional(aiko::WHITE, {-0.4f, -1.0f, -0.3f}, 1.0f);
        sun->intensity = .75f;
        sun->castsShadows = true;

        m_lightObject = Instantiate("Point");
        m_lightObject->transform().position.y = 1.5f;
        auto* point = m_lightObject->addComponent<aiko::LightComponent>();
        point->setPointLight(aiko::RED, 3.0f);
        point->intensity = 1.0f;
        point->castsShadows = true;

        auto* ground = Instantiate("Ground");

        ground->transform().position = {0.0f, -0.04f, 0.0f};
        ground->transform().scale = {5.0f, 0.05f, 5.0f};

        auto* groundMesh = ground->addComponent<aiko::MeshComponent>();

        groundMesh->loadPrimitive(aiko::MeshComponent::MeshPrimitive::Cube);

        aiko::Material& material = groundMesh->getMaterial();

        material.m_shaderId = assets().loadShader("model");
        material.m_baseColor = aiko::GRAY;
        material.m_useVertexColor = false;
        material.m_lit = true;

        m_model = assets().loadModel("project://Rumba_Dancing.fbx");
        m_modelB = assets().loadModel("project://Rumba_Dancing.fbx");

    }

    void SkeletonLab::update()
    {
        if (!m_model.isValid() || !m_modelB.isValid())
        {
            return;
        }

        if (!m_playbackStarted)
        {
            AIKO_ASSERT(m_model.animationCount() > 0, "FBX has no animation clips");

            const std::string_view clipName = m_model.animationName(0);

            AIKO_ASSERT(m_model.playAnimation(clipName), "Failed to select animation by name");

            m_externalPose = m_modelB.createBindPose();

            m_playbackStarted = true;
        }

        const float dt = getDeltaTime();

        // Character A: imported FBX animation.
        m_model.updateAnimation(dt);

        // Character B: external pose input simulation.
        m_externalPoseTime += dt;

        const auto arm = m_modelB.findJointIndex("LeftArm");

        AIKO_ASSERT(arm.has_value(), "LeftArm not found");

        const float angle = std::sin(m_externalPoseTime * 2.0f) * 0.7f;

        m_externalPose.localTransforms[*arm] = aiko::math::rotate(m_modelB.joints()[*arm].localBindTransform, angle, {0.0f, 0.0f, 1.0f});

        AIKO_ASSERT(m_modelB.setPose(m_externalPose), "Failed to apply external skeleton pose");
    }


    void SkeletonLab::render()
    {
        if (!m_model.isValid() || !m_modelB.isValid())
        {
            return;
        }

        aiko::Transform left;
        left.position = {-0.55f, 0.0f, 0.0f};
        left.scale = {0.0075f};

        aiko::Transform right;
        right.position = {0.55f, 0.0f, 0.0f};
        right.scale = {0.0075f};

        renderer().drawModel(left, m_model);
        renderer().drawModel(right, m_modelB);
    }

}

