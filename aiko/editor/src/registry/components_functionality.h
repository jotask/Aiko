#pragma once

#include <aiko_types.h>

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

namespace aiko::editor
{
    class EditorContext;
}

namespace aiko::editor::component
{

    vector<string> getMissingComponents(GameObject*);
    void addComponent(EditorContext& context, string name, GameObject& object);
    void removeComponent(EditorContext& context, Component& component);

    bool serializeComponent(const Component& component, YAML::Node& node);
    bool deserializeComponent(const YAML::Node& node, GameObject& object);

}
