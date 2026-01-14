#include "texture_component.h"

#include "models/mesh_factory.h"
#include "models/game_object.h"
#include "systems/render_system.h"
#include "constants.h"

namespace aiko
{

    TextureComponent::TextureComponent()
        : Component("Texture")
    {
    }

    void TextureComponent::init()
    {
        m_renderSystem = gameobject->getSystem<RenderSystem>();
        m_shader.load("aiko");
        AIKO_ASSERT(m_shader.isValid(), "Shader is invalid");
        const Mesh::MeshData data = mesh::factory::generateQuad();
        m_mesh.setData(data);
        AIKO_ASSERT(m_mesh.isValid(), "Mesh is invalid");
        m_texture.load("texel_checker.png");
        AIKO_ASSERT(m_texture.isValid(), "Texture is invalid");
    }

    void TextureComponent::update()
    {

    }

    void TextureComponent::render()
    {
        m_renderSystem->render(gameobject->transform(), m_mesh, m_shader, m_texture);
    }

}
