#include "sprite_component.h"

#include "models/game_object.h"
#include "systems/render_system.h"
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
        m_mesh = m_renderSystem->createMesh(Mesh::MeshType::QUAD);
        m_shader = m_renderSystem->createShader(global::getAssetPath("shaders/aiko_default_texture.vs").c_str(), global::getAssetPath("shaders/aiko_default_texture.fs").c_str());
        m_texture = m_renderSystem->createTexture();
    }

    void SpriteComponent::update()
    {
        aiko::AikoPtr<Transform> transform = gameobject->transform();
    }

    void SpriteComponent::render()
    {
        m_renderSystem->render(gameobject->transform().get(), m_mesh.get(), m_shader.get(), &m_texture);
    }

}
