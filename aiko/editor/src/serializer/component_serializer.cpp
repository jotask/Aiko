#include "component_serializer.h"

#include <vector>

#include <serializer/nodes/core_nodes_ymal.h>
#include <serializer/nodes/component_nodes_ymal.h>

#include "components/camera_component.h"
#include "components/light_component.h"
#include "components/mesh_component.h"
#include "components/sprite_component.h"
#include "components/transform_component.h"
#include "models/game_object.h"

namespace aiko::editor::serializer
{

    static const std::vector<ComponentEditorEntry> s_componentEntries =
    {
        {
            "TransforComponent",
            [](Component* c, YAML::Node& node) -> bool
            {
                if (auto* t = dynamic_cast<TransforComponent*>(c))
                {
                    node["type"] = "TransforComponent";
                    node["data"] = *t;
                    return true;
                }
                return false;
            },
            [](const YAML::Node& node, GameObject& obj) -> bool
            {
                if (!node["type"] || node["type"].as<std::string>() != "TransforComponent")
                {
                    return false;
                }

                AikoPtr<TransforComponent> t = obj.addComponent<TransforComponent>();

                AIKO_NOT_IMPLEMENTED;
                return true;
            }
        },
        {
            "TransforComponent",
            [](Component* c, YAML::Node& node) -> bool
            {
                if (auto* t = dynamic_cast<CameraComponent*>(c))
                {
                    node["type"] = "CameraComponent";
                    node["data"] = *t;
                    return true;
                }
                return false;
            },
            [](const YAML::Node& node, GameObject& obj) -> bool
            {
                if (!node["type"] || node["type"].as<std::string>() != "CameraComponent")
                {
                    return false;
                }

                AikoPtr<CameraComponent> t = obj.addComponent<CameraComponent>();

                AIKO_NOT_IMPLEMENTED;
                return true;
            }
        },
        {
            "MeshComponent",
            [](Component* c, YAML::Node& node) -> bool
            {
                if (auto* t = dynamic_cast<MeshComponent*>(c))
                {
                    node["type"] = "MeshComponent";
                    node["data"] = *t;
                    return true;
                }
                return false;
            },
            [](const YAML::Node& node, GameObject& obj) -> bool
            {
                if (!node["type"] || node["type"].as<std::string>() != "MeshComponent")
                {
                    return false;
                }

                AikoPtr<MeshComponent> t = obj.addComponent<MeshComponent>();

                AIKO_NOT_IMPLEMENTED;
                return true;
            }
        }
    };

    YAML::Node serializeComponent(Component* compt)
    {
        for (const auto& entry : s_componentEntries)
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
        for (const auto& entry : s_componentEntries)
        {
            if (entry.deserialize(node, obj))
            {
                return;
            }
        }
        AIKO_ASSERT(false, "ERROR :: Component is not supported by the editor");
    }
}
