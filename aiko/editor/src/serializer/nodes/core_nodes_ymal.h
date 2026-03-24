#pragma once

#include <aiko_types.h>

#include <math/math_vector.h>

#include <yaml-cpp/yaml.h>

namespace YAML
{

    template<>
    struct convert<aiko::vec3>
    {
        static Node encode(const aiko::vec3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node& node, aiko::vec3& rhs)
        {
            if (node.IsSequence() == false || node.size() != 3)
            {
                return false;
            }
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

}