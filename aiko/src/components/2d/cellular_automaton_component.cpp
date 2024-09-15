#include "cellular_automaton_component.h"

#include <assert.h>
#include <chrono>

#include "models/game_object.h"
#include "systems/render_system.h"
#include "constants.h"

#include "core/log.h"

namespace aiko
{

    CellularAutomatonComponent::CellularAutomatonComponent()
        : Component("CellularAutomaton")
    {

    }

    void CellularAutomatonComponent::init()
    {
        m_world.init();
        m_render.init(gameobject->getSystem<RenderSystem>());
    }

    void CellularAutomatonComponent::update()
    {
        static auto lastTime = std::chrono::steady_clock::now();
        static double accumulatedTime = 0.0;
        static const double interval = 10 / 60.0f;

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
        m_render.render(&m_world);
    }

}
