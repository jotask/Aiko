#include "cellular_automaton.h"

#include <aiko_includes.h>

#include "cell_automaton_component/cellular_automaton_component.h"

namespace aiko::ca
{

    void CellularAutomaton::init()
    {

        auto cameraObj = app->Instantiate("Camera");
        auto camera = cameraObj->addComponent<CameraComponent>(camera::CameraController::Fly, Camera::CameraType::Perspective);
        camera->getCamera().position.z = 50.0f;
        camera->getCamera().position.y = 10.0f;

        auto sprite = app->Instantiate("CellularAutomaton");
        sprite->transform().position = { 0.0f, 0.0f, 0.0f };
        sprite->transform().rotation = { 0.0f,  0.0f, 0.0f };
        sprite->transform().scale = { 1.0f, 1.0f, 1.0f };
        auto grid = sprite->addComponent<CellularAutomatonComponent>();

    }

}

