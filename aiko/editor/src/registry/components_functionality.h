#pragma once

#include <aiko_types.h>

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

    vector<string> getMissingComponents(GameObject*);
    void addComponent(string, GameObject*);
    void removeComponent(string, GameObject*);

}
