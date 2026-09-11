#pragma once

#include <models/component.h>

#include <yaml-cpp/yaml.h>

namespace aiko::editor::serializer
{

    YAML::Node serializeComponent(Component*);
    void deserializeComponent(Component*);

}
