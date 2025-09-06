#include "pbo_texture_component.h"

#include  <algorithm>
#include <assert.h>
#include <chrono>

#include "shared/math.h"
#include "shared/math_transform.h"
#include "core/utils.h"

#include "models/game_object.h"
#include "systems/render_system.h"

#include "constants.h"

namespace aiko
{
    PboTextureComponent::PboTextureComponent() : PboTextureComponent("PboTexture", 128, 128, true) { }

    PboTextureComponent::PboTextureComponent(string str, uint32_t width, uint32_t height, bool autoRender)
        : Component(str)
        , auto_render(autoRender)
    {
        m_texture.texture.width = width;
        m_texture.texture.height = height;
        auto_render = autoRender;
    }


    void PboTextureComponent::init()
    {
        m_renderSystem = gameobject->getSystem<RenderSystem>();
        if (auto_render)
        {
            m_mesh = m_renderSystem->createMesh(Mesh::MeshType::QUAD);
            m_shader.load(global::getAssetPath("shaders/aiko_default_texture.vs").c_str(), global::getAssetPath("shaders/aiko_default_texture.fs").c_str());
            assert(m_shader.isvalid() && "Shader is invalid");
        }
        m_texture = m_renderSystem->createPboTexture(m_texture.texture.width, m_texture.texture.height);
        pixels.reserve(m_texture.texture.width * m_texture.texture.height);
        pixels.resize(m_texture.texture.width * m_texture.texture.height);
        std::fill(pixels.begin(), pixels.end(), BLACK);
        is_dirty = true;
    }

    void PboTextureComponent::update()
    {

        static auto lastTime = std::chrono::steady_clock::now();
        static double accumulatedTime = 0.0;
        static const double interval = 1 / 60.0f;

        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> delta = currentTime - lastTime;
        lastTime = currentTime;

        accumulatedTime += delta.count();
        bool should_update = false;

        if (accumulatedTime >= interval)
        {
            accumulatedTime -= interval; // Handle possible overflow
            should_update = true;
        }

        if (auto_render && should_update)
        {

            struct Particle
            {
                ivec2 pos;
                ivec2 dir;
                Color col;
            };

            constexpr auto N_PARTICLES = 1000;
            constexpr bool S_CLEAR_BRACKGROUND = false;

            static std::vector<Particle> s_particles;

            const int w = m_texture.texture.width - 1;
            const int h = m_texture.texture.height - 1;

            if (s_particles.size() != N_PARTICLES)
            {
                for (uint i = 0 ; i < N_PARTICLES; i++)
                {
                    ivec2 pos = ivec2(utils::getRandomValue(0, w), utils::getRandomValue(0, h));
                    ivec2 dir = ivec2(utils::getRandomValue(-1, 1), utils::getRandomValue(-1, 1));
                    Color col = { utils::getRandomValue(0.0f, 1.0f) , utils::getRandomValue(0.0f, 1.0f) , utils::getRandomValue(0.0f, 1.0f) , 1.0f};
                    s_particles.push_back({pos, dir, col});
                }
            }
            else
            {
                for(auto& it : s_particles)
                {

                    {
                        if (it.pos.x == 0 || it.pos.x == w)
                        {
                            it.dir.x *= -1;
                        }
                        if (it.pos.y == 0 || it.pos.y == h)
                        {
                            it.dir.y *= -1;
                        }
                    }

                    it.pos.x += it.dir.x;
                    it.pos.y += it.dir.y;

                    it.pos.x = math::clamp(it.pos.x, 0, w);
                    it.pos.y = math::clamp(it.pos.y, 0, h);
                }
            }

            if (S_CLEAR_BRACKGROUND)
            {
                std::fill(pixels.begin(), pixels.end(), BLACK);
            }

            for (auto it : s_particles)
            {
                updatePixel(it.pos.x, it.pos.y, it.col);
            }

        }
    }

    void PboTextureComponent::render()
    {
        refreshPixels();
        if (auto_render == false)
        {
            return;
        }
        AIKO_DEBUG_BREAK
        // m_renderSystem->render(gameobject->transform().get(), &m_mesh, &m_shader, &m_texture);
    }

    void PboTextureComponent::refreshPixels(bool force)
    {
        if (force == true || is_dirty == true)
        {
            is_dirty = false;
            m_renderSystem->updatePbo(m_texture, pixels);
        }
    }

    void PboTextureComponent::updatePixel(uint16_t x, uint16_t y, Color c)
    {
        const uint16_t index = y * m_texture.texture.width + x;
        if (pixels[index] == c)
        {
            return;
        }
        pixels[index] = c;
        is_dirty = true;
    }

    void PboTextureComponent::updatePixels(std::vector<Color> ps)
    {
        AIKO_ASSERT(pixels.size() == ps.size(), "New pixels don't match texture size");
        pixels.clear();
        pixels.insert(pixels.end(), ps.begin(), ps.end());
        is_dirty = true;
    }

    const texture::PboTexture& PboTextureComponent::getPboTexture()
    {
        return m_texture;
    }

}
