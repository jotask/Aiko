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
        m_mesh = m_renderSystem->createMesh(Mesh::MeshType::QUAD);
        m_shader = m_renderSystem->createShader(global::getAssetPath("shaders/aiko_default_texture.vs"), global::getAssetPath("shaders/aiko_default_texture.fs"));
        m_texture = m_renderSystem->createTexture();
    }

    void TextureComponent::update()
    {
        aiko::AikoPtr<Transform> transform = gameobject->transform();
    }

    void TextureComponent::render()
    {
        m_renderSystem->render(gameobject->transform().get(), m_mesh.get(), m_shader.get(), &m_texture);
    }

}
