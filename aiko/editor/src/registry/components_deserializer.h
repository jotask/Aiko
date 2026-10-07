#pragma once

#include <yaml-cpp/yaml.h>

namespace aiko
{
    class TransformComponent;
    class SpriteComponent;
    class MeshComponent;
    class ModelComponent;
    class LightComponent;
    class CameraComponent;
}

namespace aiko::editor::component
{
    bool deserializeTransform(const YAML::Node& node,TransformComponent* component);
    bool deserializeSprite(const YAML::Node& node,SpriteComponent* component);
    bool deserializeMesh(const YAML::Node& node, MeshComponent* component);
    bool deserializeModel(const YAML::Node& node, ModelComponent* component);
    bool deserializeLight(const YAML::Node& node,LightComponent* component);
    bool deserializeCamera(const YAML::Node& node, CameraComponent* component);

}