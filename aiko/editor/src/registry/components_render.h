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
}

namespace aiko::editor::component
{

    void drawComponent(Component*);

    void drawTransform(TransformComponent*);
    void drawSprite(SpriteComponent*);
    void drawMesh(MeshComponent*);
    void drawLight(LightComponent*);
    void drawCamera(CameraComponent*);

}
