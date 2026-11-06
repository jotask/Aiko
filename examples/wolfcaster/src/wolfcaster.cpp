#include "wolfcaster.h"

#include "core/file.h"
#include "layers/contexts/scene_context.h"

#include <components/camera_component.h>
#include <components/mesh_component.h>
#include <intrumentor/profiler.h>
#include <magic_enum/magic_enum.hpp>
#include <types/draw_types.h>

#include <imgui.h>
#include <random>
#include <vector>

namespace wc
{

    WolfCaster::WolfCaster() = default;

    WolfCaster::~WolfCaster() = default;

    void WolfCaster::init()
    {

        scene().clearColor() = {0.10f, 0.16f, 0.13f};

        aiko::CameraComponent* camera = scene().createCamera(aiko::camera::CameraController::Fly, aiko::Camera::CameraType::Perspective);
        camera->getCamera().position = { 0.0f, 50.0f, 10.0f };
        camera->getCamera().target = {0.0f, 0.0f, 0.0f};
        camera->speed() *= 4.0f;

        scene().ambientLight().color = aiko::WHITE;
        scene().ambientLight().intensity = 0.3f;

    }

    void WolfCaster::update()
    {
        AIKO_FUNCTION_PROFILE
    }

    void WolfCaster::render()
    {
        AIKO_FUNCTION_PROFILE
    }

}

