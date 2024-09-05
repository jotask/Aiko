#include "cellular_automaton_component.h"

#include <assert.h>
#include <chrono>

#include "models/game_object.h"
#include "systems/render_system.h"
#include "constants.h"

namespace aiko
{

    CellularAutomatonComponent::CellularAutomatonComponent()
        : Component("CellularAutomaton")
    {
        m_texture.texture.width = cellautomaton::SIZE_CHUNK;
        m_texture.texture.height = cellautomaton::SIZE_CHUNK;
    }

    void CellularAutomatonComponent::init()
    {
        m_renderSystem = gameobject->getSystem<RenderSystem>();
        m_mesh = m_renderSystem->createMesh(Mesh::MeshType::QUAD);
        m_shader = m_renderSystem->createShader(global::getAssetPath("shaders/aiko_default_texture.vs").c_str(), global::getAssetPath("shaders/aiko_default_texture.fs").c_str());
        m_texture = m_renderSystem->createPboTexture(m_texture.texture.width, m_texture.texture.height);
        pixels.reserve(m_texture.texture.width * m_texture.texture.height);
        pixels.resize(m_texture.texture.width * m_texture.texture.height);
        std::fill(pixels.begin(), pixels.end(), BLACK);
        is_dirty = true;
        m_world.init();
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
            updatePixels(pixels);
            refreshPixels();
            trans.position = chunk.getPosition();
            m_renderSystem->render(&trans, m_mesh.get(), m_shader.get(), &m_texture.texture);
        }
    }

    void CellularAutomatonComponent::refreshPixels(bool force)
    {
        if (force == true || is_dirty == true)
        {
            is_dirty = false;
            m_renderSystem->updatePbo(m_texture, pixels);
        }
    }

    void CellularAutomatonComponent::updatePixel(uint16_t x, uint16_t y, Color c)
    {
        const uint16_t index = y * m_texture.texture.width + x;
        if (pixels[index] == c)
        {
            return;
        }
        pixels[index] = c;
        is_dirty = true;
    }

    void CellularAutomatonComponent::updatePixels(std::vector<Color> ps)
    {
        // assert(pixels.size() == ps.size(), "New pixels don't match texture size");
        pixels.clear();
        pixels.insert(pixels.end(), ps.begin(), ps.end());
        is_dirty = true;
    }

}
