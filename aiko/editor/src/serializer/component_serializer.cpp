#include "component_serializer.h"

#include <serializer/nodes/core_nodes_ymal.h>
#include <serializer/nodes/component_nodes_ymal.h>

#include "components/camera_component.h"
#include "components/light_component.h"
#include "components/mesh_component.h"
#include "components/sprite_component.h"
#include "components/transform_component.h"
#include "registry/component_registry.h"
#include "models/game_object.h"

namespace aiko::editor::serializer
{

    YAML::Node serializeComponent(Component* compt)
    {
        for (const auto& entry : component::s_componentEntries)
        {
            YAML::Node node(YAML::NodeType::Map);
            if (entry.serialize(compt, node))
            {
                return node;
            }
        }
        AIKO_ASSERT(false, "ERROR :: Component is not supported by the editor");
        return {};
    }

    void deserializeComponent(const YAML::Node& node, GameObject& obj)
    {
        for (const auto& entry : component::s_componentEntries)
        {
            if (entry.deserialize(node, obj))
            {
                return;
            }
        }
        AIKO_ASSERT(false, "ERROR :: Component is not supported by the editor");
    }

}
