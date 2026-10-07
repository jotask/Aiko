#include "components_functionality.h"

#include "models/component.h"
#include "models/game_object.h"
#include "registry/component_registry.h"

namespace aiko::editor
{
    namespace component
    {

        vector<string> getMissingComponents(GameObject* object)
        {
            vector<string> result;

            AIKO_ASSERT(object != nullptr, "Cannot query components for null GameObject");

            if (object == nullptr)
            {
                return result;
            }

            for (const ComponentEditorEntry& entry : componentEntries())
            {
                if (entry.addable && entry.has(*object) == false)
                {
                    result.push_back(entry.name);
                }
            }

            return result;
        }

        void addComponent(EditorContext& context, string name, GameObject& object)
        {
            const ComponentEditorEntry* entry = findComponentEntryByName(name);

            AIKO_ASSERT(entry != nullptr, "Component is not supported by the editor");

            if (entry == nullptr)
            {
                return;
            }

            AIKO_ASSERT(entry->addable, "Component cannot be added");

            if (entry->addable == false)
            {
                return;
            }

            const bool hasComponent = entry->has(object);

            AIKO_ASSERT(hasComponent == false, "GameObject already has this component");

            if (hasComponent)
            {
                return;
            }

            entry->add(context, object);
        }

        void removeComponent(EditorContext& context, Component& component)
        {
            const ComponentEditorEntry* entry = findComponentEntry(component);

            AIKO_ASSERT(entry != nullptr, "Component is not supported by the editor");

            if (entry == nullptr)
            {
                return;
            }

            AIKO_ASSERT(entry->removable, "Component cannot be removed");

            if (entry->removable == false)
            {
                return;
            }

            GameObject* object = component.getGameObject();

            AIKO_ASSERT(object != nullptr, "Component is not attached to a GameObject");

            if (object == nullptr)
            {
                return;
            }

            entry->remove(context, *object);
        }

        bool serializeComponent(const Component& component, YAML::Node& node)
        {
            const ComponentEditorEntry* entry = findComponentEntry(component);

            if (entry == nullptr)
            {
                return false;
            }

            node = YAML::Node(YAML::NodeType::Map);

            node["type"] = entry->serializedName;

            node["data"] = entry->serialize(component);

            return true;
        }

        bool deserializeComponent(const YAML::Node& node, GameObject& object)
        {
            if (!node["type"])
            {
                return false;
            }

            const string serializedName = node["type"].as<string>();

            const ComponentEditorEntry* entry = findComponentEntryBySerializedName(serializedName);

            if (entry == nullptr)
            {
                return false;
            }

            return entry->deserialize(node["data"], object);
        }

    }
}