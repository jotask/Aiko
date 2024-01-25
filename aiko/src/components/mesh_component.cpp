#include "mesh_component.h"

#include "models/game_object.h"
#include "systems/render_system.h"

MeshComponent::MeshComponent()
    : Component("Mesh")
{

}

void MeshComponent::init()
{
    m_renderSystem = gameobject->getSystem<RenderSystem>().get();
    m_mesh = m_renderSystem->createMesh();
}

void MeshComponent::update()
{
    aiko::AikoPtr<Transform> transform = gameobject->transform();
}

void MeshComponent::render()
{
    m_renderSystem->render(this);
}

void MeshComponent::setPrimitive(Mesh::Primitive prm)
{
    m_mesh->setPrimitive(prm);
}
