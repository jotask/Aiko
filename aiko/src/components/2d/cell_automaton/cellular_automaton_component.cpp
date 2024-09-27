#include "cellular_automaton_component.h"

#include <assert.h>
#include <chrono>

#include <aiko_includes.h>

namespace aiko::ca
{

    CellularAutomatonComponent::CellularAutomatonComponent()
        : Component("CellularAutomaton")
    {

    }

    void CellularAutomatonComponent::init()
    {
        m_world.init();
        // m_render.init(gameobject->getSystem<RenderSystem>());
    }

    void CellularAutomatonComponent::update()
    {

        if (cellautomaton::WORLD_FPS_TIMER_LOCK == false)
        {

            aiko::InputSystem* input = gameobject->getSystem<InputSystem>();

            if (input->isKeyJustPressed(KEY_SPACE))
            {
                m_world.update();
            }
            return;
        }

        static auto lastTime = std::chrono::steady_clock::now();
        static double accumulatedTime = 0.0;
        static const double interval = cellautomaton::WORLD_FRAME_RATE / 60.0f;

        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> delta = currentTime - lastTime;
        lastTime = currentTime;

        accumulatedTime += delta.count();
        bool should_update = false;

        if (accumulatedTime >= interval)
        {
            accumulatedTime -= interval;
            m_world.update();
        }
    }

    void CellularAutomatonComponent::render()
    {
        // m_render.render(&m_world);
    }

}
