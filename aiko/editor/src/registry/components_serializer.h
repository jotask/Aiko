#pragma once

#include <yaml-cpp/yaml.h>

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

    void serializeComponent(Component*);

    YAML::Node serializeTransform(const TransformComponent*);
    YAML::Node serializeSprite(const SpriteComponent*);
    YAML::Node serializeMesh(const MeshComponent*);
    YAML::Node serializeLight(const LightComponent*);
    YAML::Node serializeCamera(const CameraComponent*);

}
