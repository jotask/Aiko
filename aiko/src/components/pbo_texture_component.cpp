#include "pbo_texture_component.h"

#include  <algorithm>

#include "models/game_object.h"
#include "systems/render_system.h"

namespace aiko
{

    PboTextureComponent::PboTextureComponent()
        : Component("PboTexture")
    {

    }

    void PboTextureComponent::init()
    {
        m_renderSystem = gameobject->getSystem<RenderSystem>();
        m_mesh = m_renderSystem->createMesh(Mesh::MeshType::QUAD);
        m_shader = m_renderSystem->createShader("C:/Users/j.iznardo/Documents/Aiko/assets/shaders/aiko_default_texture.vs", "C:/Users/j.iznardo/Documents/Aiko/assets/shaders/aiko_default_texture.fs");
        m_texture = m_renderSystem->createPboTexture(128, 128);

        pixels.reserve(m_texture.texture.width * m_texture.texture.height);
        pixels.resize(m_texture.texture.width * m_texture.texture.height);

        std::fill(pixels.begin(), pixels.end(), BLACK);

        is_dirty = true;

    }

    void PboTextureComponent::update()
    {
        static int x = 0;
        static int y = m_texture.texture.height / 2;
        std::fill(pixels.begin(), pixels.end(), BLACK);
        updatePixel(x, y, WHITE);
        x++;
        x %= m_texture.texture.width;
    }

    void PboTextureComponent::render()
    {
        if(is_dirty == true)
        {
            is_dirty = false;
            m_renderSystem->updatePbo(m_texture, pixels);
        }
        m_renderSystem->render(gameobject->transform().get(), m_mesh.get(), m_shader.get(), &m_texture.texture);
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

}
