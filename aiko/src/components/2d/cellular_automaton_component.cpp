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
        m_renderSystem = gameobject->getSystem<RenderSystem>();
        m_mesh = m_renderSystem->createMesh(Mesh::MeshType::QUAD);
        m_shader = m_renderSystem->createShader(global::getAssetPath("shaders/aiko_default_texture.vs").c_str(), global::getAssetPath("shaders/aiko_default_texture.fs").c_str());
        m_world.init(m_renderSystem);
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
        Transform trans;
        for (auto& chunk : m_world.getChunks())
        {
            auto pixels = chunk.getPixels();
            auto pbo = chunk.getPbo();
            trans.position = chunk.getPosition();
            m_renderSystem->render(&trans, m_mesh.get(), m_shader.get(), &pbo.texture);
        }
    }

}
