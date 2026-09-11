#pragma once

#include <aiko_types.h>

#include <math/math_vector.h>

#include <yaml-cpp/yaml.h>

#include "core/uuid.h"
#include "types/color.h"

namespace YAML
{

    template<>
    struct convert<aiko::uuid::Uuid>
    {
        static Node encode(const aiko::uuid::Uuid& rhs)
        {
            return Node(rhs.get().c_str());
        }

        static bool decode(const Node& node, aiko::uuid::Uuid& rhs)
        {
            if (node.IsScalar() == false)
            {
                return false;
            }
            rhs = aiko::uuid::Uuid(node.as<std::string>());
            return true;
        }
    };

    template<>
    struct convert<aiko::vec3>
    {
        static Node encode(const aiko::vec3& rhs)
        {
            Node node(NodeType::Map);
            node["x"] = rhs.x;
            node["y"] = rhs.y;
            node["z"] = rhs.z;
            return node;
        }

        static bool decode(const Node& node, aiko::vec3& rhs)
        {
            if (node.IsMap() == false || node.size() != 3)
            {
                return false;
            }
            rhs.x = node["x"].as<float>();
            rhs.y = node["y"].as<float>();
            rhs.z = node["z"].as<float>();
            return true;
        }
    };



    template<>
    struct convert<aiko::Color>
    {
        static Node encode(const aiko::Color& rhs)
        {
            Node node(NodeType::Map);
            node["r"] = rhs.r;
            node["g"] = rhs.g;
            node["b"] = rhs.b;
            node["a"] = rhs.a;
            return node;
        }

        static bool decode(const Node& node, aiko::Color& rhs)
        {
            if (node.IsScalar() == false)
            {
                return false;
            }
            AIKO_NOT_IMPLEMENTED;
            return true;
        }
    };

}
