#include "visual_lab.h"

#include "layers/contexts/scene_context.h"

namespace aiko::lab
{

    void VisualLab::connect(SystemConnector& systemConnector)
    {

    }

    void VisualLab::init()
    {
        scene().clearColor() = RAYWHITE;
        scene().ambientLight().color = WHITE;
        scene().ambientLight().intensity = 0.15f;

        aiko::CameraComponent* camera = scene().createCamera(aiko::camera::CameraController::Fly);
        camera->getCamera().position = { 0.0f, 2.5f, 8.0f };

    }

    void VisualLab::update()
    {

    }

    void VisualLab::render()
    {

    }

    void VisualLab::dispose()
    {

    }

}
