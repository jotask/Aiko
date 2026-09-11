#pragma once

namespace aiko
{
    class GameObject;
    class Component;
    class TransforComponent;
    class SpriteComponent;
    class MeshComponent;
    class LightComponent;
    class CameraComponent;
}

namespace aiko::editor::component
{

    void deserializeComponent(Component*);

    void deserializeTransform(TransforComponent*);
    void deserializeSprite(SpriteComponent*);
    void deserializeMesh(MeshComponent*);
    void deserializeLight(LightComponent*);
    void deserializeCamera(CameraComponent*);

}
