#pragma once

#include <aiko_types.h>
#include <vector>

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

    void drawComponent(Component*);

    void drawTransform(TransforComponent*);
    void drawSprite(SpriteComponent*);
    void drawMesh(MeshComponent*);
    void drawLight(LightComponent*);
    void drawCamera(CameraComponent*);

}
