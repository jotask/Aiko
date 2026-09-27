#pragma once

#include <aiko_types.h>

#include <yaml-cpp/yaml.h>

#include <functional>
#include <string_view>
#include <typeindex>

namespace aiko
{
    class GameObject;
    class Component;
}

namespace aiko::editor
{
    class EditorContext;
}

namespace aiko::editor::component
{

    struct ComponentEditorEntry
    {
        string name;
        string serializedName;
        std::type_index type;

        bool addable;
        bool removable;

        std::function<bool(const GameObject&)> has;

        std::function<void(EditorContext&, GameObject&)> add;

        std::function<void(Component&)> render;

        std::function<YAML::Node(const Component&)> serialize;

        std::function<bool(const YAML::Node&, GameObject&)> deserialize;
    };

    const vector<ComponentEditorEntry>& componentEntries();

    const ComponentEditorEntry* findComponentEntry(const Component& component);

    const ComponentEditorEntry* findComponentEntryByName(std::string_view name);

    const ComponentEditorEntry* findComponentEntryBySerializedName(std::string_view serializedName);

}