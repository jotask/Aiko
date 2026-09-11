#pragma once

#include <aiko_types.h>

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

    vector<string> getMissingComponents(GameObject*);
    void addComponent(string, GameObject*);

}
