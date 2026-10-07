#include "cellular_automaton.h"

#include "layers/layer_context.h"

#include <aiko_includes.h>

namespace aiko::ca
{

    void CellularAutomaton::init()
    {

        scene().clearColor() = aiko::SKYBLUE;

        const ivec2 worldSize =
        {
            SIZE_WORLD.x * SIZE_CHUNK.x,
            SIZE_WORLD.y * SIZE_CHUNK.y
        };

        const vec2 worldCenter =
        {
            (static_cast<float>(worldSize.x) - 1.0f) * 0.5f,
            (static_cast<float>(worldSize.y) - 1.0f) * 0.5f
        };

        auto* camera = scene().createCamera(camera::CameraController::Drag, Camera::CameraType::Orthographic);
        camera->getCamera().position = { worldCenter.x, worldCenter.y, 100.0f };
        camera->getCamera().target = { worldCenter.x, worldCenter.y, 0.0f };
        camera->getCamera().m_orthoHeight = static_cast<float>(worldSize.y) + 4.0f;

        m_world.init();

        m_renderer.init(context());

    }

    void CellularAutomaton::update()
    {
        if constexpr (WORLD_FPS_TIMER_LOCK == false)
        {
            if (context().input().isKeyJustPressed(Key::KEY_SPACE))
            {
                m_world.update();
            }
            return;
        }

        static double accumulatedTime = 0.0;
        static const double interval = 1.0 / WORLD_FRAME_RATE;

        accumulatedTime += getDeltaTime();

        while (accumulatedTime >= interval)
        {
            accumulatedTime -= interval;
            m_world.update();
        }

    }

    void CellularAutomaton::render()
    {
        m_renderer.render(&m_world);
    }

}

