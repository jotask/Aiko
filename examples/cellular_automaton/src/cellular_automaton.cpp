#include "cellular_automaton.h"

#include <aiko_includes.h>

#include "cell_automaton_component/cellular_automaton_component.h"

namespace aiko::ca
{

    void CellularAutomaton::init()
    {
        Application::init();

        auto cameraObj = this->Instantiate("Camera");
        auto camera = cameraObj->addComponent<CameraComponent>(camera::CameraController::Fly, camera::CameraType::Perspective);
        camera->getCamera()->position.z = 50.0f;
        camera->getCamera()->position.y = 10.0f;

        m_sprite = this->Instantiate("CellularAutomaton");
        m_sprite->transform()->position = { 0.0f, 0.0f, 0.0f };
        m_sprite->transform()->rotation = { 0.0f,  0.0f, 0.0f };
        m_sprite->transform()->scale = { 1.0f, 1.0f, 1.0f };
        auto grid = m_sprite->addComponent<CellularAutomatonComponent>();

    }

    void CellularAutomaton::update()
    {
        Application::update();
    }

    void CellularAutomaton::render()
    {
        Application::render();
    }
}

