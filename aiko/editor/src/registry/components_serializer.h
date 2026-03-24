#pragma once

#include <yaml-cpp/yaml.h>

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

    void serializeComponent(Component*);

    YAML::Node serializeTransform(TransforComponent*);
    YAML::Node serializeSprite(SpriteComponent*);
    YAML::Node serializeMesh(MeshComponent*);
    YAML::Node serializeLight(LightComponent*);
    YAML::Node serializeCamera(CameraComponent*);

}
