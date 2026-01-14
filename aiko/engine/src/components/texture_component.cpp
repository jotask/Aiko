#include "texture_component.h"

#include "models/mesh_factory.h"
#include "models/game_object.h"
#include "systems/render_system.h"
#include "constants.h"

#include <chrono>

namespace aiko
{
    TextureComponent::TextureComponent(TextureMode mode)
        : Component("Texture")
        , m_textureMode(mode)
    {
    }

    TextureComponent::TextureComponent()
        : TextureComponent(TextureMode::PBO)
    {
    }

    TextureComponent::TextureComponent(string file)
        : TextureComponent(TextureMode::FILE)
    {
        m_filePath = file;
    }

    void TextureComponent::init()
    {
        m_renderSystem = gameobject->getSystem<RenderSystem>();
        m_shader.load("aiko");
        AIKO_ASSERT(m_shader.isValid(), "Shader is invalid");
        const Mesh::MeshData data = mesh::factory::generateQuad();
        m_mesh.setData(data);
        AIKO_ASSERT(m_mesh.isValid(), "Mesh is invalid");

        switch (m_textureMode)
        {
            case TextureMode::FILE:
                {
                    load(m_filePath);
                }
                break;
            case TextureMode::PBO:
                {
                    constexpr uint16_t SIZE = 128;
                    const uint16_t pixel_size = SIZE * SIZE;
                    pixels.reserve(pixel_size);
                    pixels.resize(pixel_size);
                    std::fill(pixels.begin(), pixels.end(), RAYWHITE);
                    m_texture.create(SIZE, SIZE);
                    is_dirty = true;
                }
                break;
        default:
            AIKO_ASSERT(false, "Unknown texture type")
        }

    }

    void TextureComponent::update()
    {

        if (m_textureMode != TextureMode::PBO)
        {
            return;
        }

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

        if (should_update)
        {

            struct Particle
            {
                ivec2 pos;
                ivec2 dir;
                Color col;
            };

            constexpr auto N_PARTICLES = 100;
            constexpr bool S_CLEAR_BRACKGROUND = false;

            static std::vector<Particle> s_particles;

            const int w = m_texture.getInfo().width - 1;
            const int h = m_texture.getInfo().height - 1;

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
                std::fill(pixels.begin(), pixels.end(), RAYWHITE);
            }

            for (auto it : s_particles)
            {
                setPixel(it.pos.x, it.pos.y, it.col);
            }

        }

        // Update pixels
        refresh();

    }

    void TextureComponent::render()
    {
        m_renderSystem->render(gameobject->transform(), m_mesh, m_shader, m_texture);
    }

    void TextureComponent::load(string file)
    {

        AIKO_ASSERT(file.empty() == false, "Attempting to load empty file")

        if (m_texture.isValid() == true)
        {
            m_texture.unload();
        }
        m_textureMode = TextureMode::FILE;
        m_filePath = file;
        m_texture.load(m_filePath);
        AIKO_ASSERT(m_texture.isValid(), "Texture is invalid");
    }

    void TextureComponent::setPixel(uint16_t x, uint16_t y, Color c)
    {
        AIKO_ASSERT(m_textureMode == TextureMode::PBO, "Texture not in PBO mode")
        const uint16_t index = y * m_texture.getInfo().width + x;
        if (pixels[index] == c)
        {
            return;
        }
        pixels[index] = c;
        is_dirty = true;
    }

    void TextureComponent::setPixels(std::vector<Color> ps)
    {
        AIKO_ASSERT(m_textureMode == TextureMode::PBO, "Texture not in PBO mode")
        AIKO_ASSERT(pixels.size() == ps.size(), "New pixels don't match texture size");
        pixels.clear();
        pixels.insert(pixels.end(), ps.begin(), ps.end());
        is_dirty = true;
    }

    void TextureComponent::refresh()
    {
        if (is_dirty == false)
        {
            return;
        }
        is_dirty = false;
        AIKO_ASSERT(m_textureMode == TextureMode::PBO, "Texture not in PBO mode")
        m_texture.setPixels(pixels);
    }

}
