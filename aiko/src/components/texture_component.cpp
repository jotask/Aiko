#include "texture_component.h"

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
        m_shader.load("aiko.vs", "aiko.fs");
        m_mesh = m_renderSystem->createMesh(Mesh::MeshType::CUBE);
        assert(m_shader.isvalid() && "Shader is invalid");
        m_texture.loadTextureFromFile("texel_checker.png");
        m_shader.preLoadUniforms({ {"u_texture", ShaderUniformDataType::SHADER_UNIFORM_SAMPLER2D} });
    }

    void TextureComponent::update()
    {
        aiko::AikoPtr<Transform> transform = gameobject->transform();
    }

    void TextureComponent::render()
    {
        m_renderSystem->render(gameobject->transform().get(), &m_mesh, &m_shader, &m_texture);
    }

}
