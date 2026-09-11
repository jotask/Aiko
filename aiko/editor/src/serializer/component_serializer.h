#pragma once

#include <models/component.h>

#include <yaml-cpp/yaml.h>

namespace aiko::editor::serializer
{

    YAML::Node serializeComponent(const Component*);
    void deserializeComponent(Component*);

}
