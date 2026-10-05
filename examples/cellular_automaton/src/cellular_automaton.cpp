#include "cellular_automaton.h"

#include "cell_automaton_component/cellular_automaton_component.h"
#include "layers/layer_context.h"

#include <aiko_includes.h>

namespace aiko::ca
{

    void CellularAutomaton::init()
    {

        scene().clearColor() = aiko::SKYBLUE;

        auto* camera = scene().createCamera(camera::CameraController::Drag, Camera::CameraType::Orthographic);
        camera->getCamera().position.z = 100.0f;

        m_automaton = sprite->addComponent<CellularAutomatonComponent>();

        m_renderer.init(context());

    }

    void CellularAutomaton::update()
    {
        if (cellautomaton::WORLD_FPS_TIMER_LOCK == false)
        {
            if (context().input().isKeyJustPressed(KEY_SPACE))
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

