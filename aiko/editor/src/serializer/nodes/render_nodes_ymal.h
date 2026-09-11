#pragma once

#include <aiko_types.h>

#include <yaml-cpp/yaml.h>

#include "serializer/nodes/core_nodes_ymal.h"
#include "serializer/nodes/render_nodes_ymal.h"
#include "assets/types/material_asset.h"
#include "metadata/material_instance.h"


namespace YAML
{

    template<>
    struct convert<aiko::MaterialAsset>
    {
        static Node encode(const aiko::MaterialAsset& rhs)
        {
            Node node (NodeType::Map);
            node["shaderId"] = rhs.shaderId;
            node["diffuseTextureId"] = rhs.diffuseTextureId;
            node["color"] = rhs.baseColor;
            node["useVertexColor"] = rhs.useVertexColor;
            node["lit"] = rhs.lit;
            return node;
        }

        static bool decode(const Node& node, aiko::MaterialAsset& rhs)
        {
            if (node.IsMap() == false)
            {
                return false;
            }
            AIKO_NOT_IMPLEMENTED;
            return true;
        }
    };

    template<>
    struct convert<aiko::MaterialInstance>
    {
        static Node encode(const aiko::MaterialInstance& rhs)
        {
            Node node(NodeType::Map);
            node["shaderId"] = rhs.shaderId;
            return node;
        }

        static bool decode(const Node& node, aiko::MaterialInstance& rhs)
        {
            if (node.IsMap() == false || node.size() != 3)
            {
                return false;
            }
            AIKO_NOT_IMPLEMENTED;
            return true;
        }
    };

}
