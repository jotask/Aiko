#include "cellular_automaton.h"

#include <aiko_includes.h>

#include "cell_automaton_component/cellular_automaton_component.h"

namespace aiko::ca
{

    void CellularAutomaton::init()
    {

        auto cameraObj = Instantiate("Camera");
        auto camera = cameraObj->addComponent<CameraComponent>(camera::CameraController::Fly, Camera::CameraType::Perspective);
        camera->getCamera().position.z = 50.0f;
        camera->getCamera().position.y = 10.0f;

        auto sprite = Instantiate("CellularAutomaton");
        sprite->transform().position = { 0.0f, 0.0f, 0.0f };
        sprite->transform().rotation = { 0.0f,  0.0f, 0.0f };
        sprite->transform().scale = { 1.0f, 1.0f, 1.0f };

        m_automaton = sprite->addComponent<CellularAutomatonComponent>();

        m_renderer.init(context());

    }

    void CellularAutomaton::update()
    {
        if (cellautomaton::WORLD_FPS_TIMER_LOCK == false)
        {
            if (isKeyJustPressed(KEY_SPACE))
            {
                m_automaton->getWorld().update();
            }
            return;
        }

        static double accumulatedTime = 0.0;
        static const double interval = cellautomaton::WORLD_FRAME_RATE / 60.0f;

        accumulatedTime += getDeltaTime();

        if (accumulatedTime >= interval)
        {
            accumulatedTime -= interval;
            m_automaton->getWorld().update();
        }

    }

    void CellularAutomaton::render()
    {
        m_renderer.render(&m_automaton->getWorld());
    }

}

