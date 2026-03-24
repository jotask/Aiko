#pragma once

#include <aiko_types.h>

#include <components/transform_component.h>

#include <yaml-cpp/yaml.h>

namespace YAML
{

    template<>
    struct convert<aiko::TransforComponent>
    {
        static Node encode(const aiko::TransforComponent& rhs)
        {
            Node node;
            AIKO_NOT_IMPLEMENTED;
            return node;
        }

        static bool decode(const Node& node, aiko::TransforComponent& rhs)
        {
            if (node.IsSequence() == false || node.size() != 3)
            {
                return false;
            }
            AIKO_NOT_IMPLEMENTED;
            return true;
        }
    };

}