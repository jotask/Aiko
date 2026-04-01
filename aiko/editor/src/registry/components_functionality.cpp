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

        void removeComponent(string name, GameObject* obj)
        {
            for (const auto& entry : s_componentEntries)
            {
                if (entry.name == name)
                {
                    entry.remove(obj);
                    return;
                }
            }
            AIKO_ASSERT(false, "ERROR :: Component is not supported by the editor");
        }

        void addComponent(string name, GameObject* obj)
        {
            for (const auto& entry : s_componentEntries)
            {
                if (entry.name == name)
                {
                    entry.add(obj);
                    return;
                }
            }
            AIKO_ASSERT(false, "ERROR :: Component is not supported by the editor");
        }

    }
}