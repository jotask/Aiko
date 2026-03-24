#pragma once

#include <aiko_types.h>
#include <models/component.h>

#include <yaml-cpp/yaml.h>

#include <functional>

namespace aiko::editor::serializer
{

    struct ComponentEditorEntry
    {
        string name;
        std::function<bool(Component*, YAML::Node&)> serialize;
        std::function<bool(const YAML::Node& node, GameObject& obj)> deserialize;
    };

    YAML::Node serializeComponent(Component*);
    void deserializeComponent(Component*);

}
