#pragma once

#include <models/game_object.h>
#include <models/component.h>

#include <yaml-cpp/yaml.h>

#include <functional>

namespace aiko::editor
{
    class EditorContext;
}

namespace aiko::editor::component
{

    struct ComponentEditorEntry
    {
        std::string name;
        std::function<bool(GameObject*)> has;
        std::function<void(EditorContext&, GameObject&)> add;
        std::function<bool(Component*)> render;
        std::function<bool(const Component* c, YAML::Node& node)> serialize;
        std::function<bool(const YAML::Node& node, GameObject& obj)> deserialize;
    };

    extern const vector<ComponentEditorEntry> s_componentEntries;

}