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

namespace aiko::editor
{

    class EditorContext;

    namespace component
    {

        bool drawComponent(EditorContext& context, Component*);

        bool drawTransform(EditorContext& context, TransformComponent*);
        bool drawSprite(EditorContext& context, SpriteComponent*);
        bool drawMesh(EditorContext& context, MeshComponent*);
        bool drawModel(EditorContext& context, ModelComponent*);
        bool drawLight(EditorContext& context, LightComponent*);
        bool drawCamera(EditorContext& context, CameraComponent*);
    }

}
