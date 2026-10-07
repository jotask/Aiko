#pragma once

namespace aiko
{
    class GameObject;
    class Component;
    class TransformComponent;
    class SpriteComponent;
    class MeshComponent;
    class LightComponent;
    class CameraComponent;
    class ModelComponent;
}

namespace aiko::editor::component
{

    bool drawComponent(Component*);

    bool drawTransform(TransformComponent*);
    bool drawSprite(SpriteComponent*);
    bool drawMesh(MeshComponent*);
    bool drawModel(ModelComponent*);
    bool drawLight(LightComponent*);
    bool drawCamera(CameraComponent*);

}
