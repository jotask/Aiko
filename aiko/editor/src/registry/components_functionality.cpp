#include "components_functionality.h"

#include "registry/component_registry.h"

namespace aiko::editor
{
    namespace component
    {

        vector<string> getMissingComponents(GameObject* obj)
        {
            vector<string> result;
            for (const auto& entry : s_componentEntries)
            {
                if (entry.has(obj) == false)
                {
                    result.push_back(entry.name);
                }
            }
            return result;
        }

        void addComponent(EditorContext& context, string name, GameObject& object)
        {
            for (const auto& entry : s_componentEntries)
            {
                if (entry.name == name)
                {
                    entry.add(context, object);
                    return;
                }
            }
            AIKO_ASSERT(false, "ERROR :: Component is not supported by the editor");
        }

        bool serializeComponent(const Component& component, YAML::Node& node)
        {
            for (const auto& entry : s_componentEntries)
            {
                if (entry.serialize(&component, node))
                {
                    return true;
                }
            }
            return false;
        }

        bool deserializeComponent(const YAML::Node& node, GameObject& object)
        {
            for (const auto& entry : s_componentEntries)
            {
                if (entry.deserialize(node, object))
                {
                    return true;
                }
            }
            return false;
        }

    }
}