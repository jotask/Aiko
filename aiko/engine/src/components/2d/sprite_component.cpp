#include "sprite_component.h"

#include "models/game_object.h"
#include "systems/render_system.h"
#include "models/mesh_factory.h"
#include "constants.h"

namespace aiko
{

    SpriteComponent::SpriteComponent()
        : Component("Sprite")
    {

    }

    void SpriteComponent::init()
    {
        m_renderSystem = gameobject->getSystem<RenderSystem>();
        const Mesh::MeshData meshData = mesh::factory::generateCube();
        m_mesh.setData(meshData);
        m_shader.load("aiko_default_texture.vs", "shaders/aiko_default_texture.fs");
        assert(m_shader.isvalid() && "Shader not valid?");
        assert(m_mesh.isValid() && "Mesh not valid?");
        AIKO_DEBUG_BREAK
        // m_texture = m_renderSystem->createTexture();
    }

    void SpriteComponent::update()
    {

    }

    void SpriteComponent::render()
    {
        m_renderSystem->render(gameobject->transform(), m_mesh, m_shader, m_texture);
    }

}
