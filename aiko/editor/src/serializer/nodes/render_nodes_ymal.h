#pragma once

#include <aiko_types.h>

#include <yaml-cpp/yaml.h>

#include "serializer/nodes/core_nodes_ymal.h"
#include "assets/types/material_asset.h"
#include "metadata/material_instance.h"
#include <models/material.h>
#include <types/render_state.h>

#include <magic_enum/magic_enum.hpp>

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

    template<>
    struct convert<aiko::RenderState>
    {
        static Node encode(const aiko::RenderState& rhs)
        {
            Node node(NodeType::Map);
            node["cullMode"] = aiko::string(magic_enum::enum_name(rhs.cullMode));
            node["fillMode"] = aiko::string(magic_enum::enum_name(rhs.fillMode));
            node["depthTest"] = rhs.depthTest;
            node["depthWrite"] = rhs.depthWrite;
            node["depthCompare"] = aiko::string(magic_enum::enum_name(rhs.depthCompare));
            node["blend"] = rhs.blend;
            return node;
        }

        static bool decode(const Node& node, aiko::RenderState& rhs)
        {
            if (node.IsMap() == false)
            {
                return false;
            }

            if (node["cullMode"])
            {
                const auto value = magic_enum::enum_cast<aiko::CullMode>(node["cullMode"].as<aiko::string>());

                if (!value)
                {
                    return false;
                }

                rhs.cullMode = *value;
            }

            if (node["fillMode"])
            {
                const auto value =
                    magic_enum::enum_cast<aiko::FillMode>(node["fillMode"].as<aiko::string>());

                if (!value)
                {
                    return false;
                }

                rhs.fillMode = *value;
            }

            if (node["depthCompare"])
            {
                const auto value =
                    magic_enum::enum_cast<aiko::DepthCompare>(node["depthCompare"].as<aiko::string>());

                if (!value)
                {
                    return false;
                }

                rhs.depthCompare = *value;
            }

            if (node["depthTest"])
            {
                rhs.depthTest = node["depthTest"].as<bool>();
            }

            if (node["depthWrite"])
            {
                rhs.depthWrite = node["depthWrite"].as<bool>();
            }

            if (node["blend"])
            {
                rhs.blend = node["blend"].as<bool>();
            }

            return true;
        }
    };

    template<>
    struct convert<aiko::Material>
    {
        static Node encode(const aiko::Material& rhs)
        {
            Node node(NodeType::Map);
            node["baseColor"] = rhs.m_baseColor;
            node["lit"] = rhs.m_lit;
            node["useVertexColor"] = rhs.m_useVertexColor;
            node["renderState"] = rhs.m_renderState;
            return node;
        }

        static bool decode(const Node& node, aiko::Material& rhs)
        {
            if (node.IsMap() == false)
            {
                return false;
            }

            if (node["baseColor"])
            {
                rhs.m_baseColor = node["baseColor"].as<aiko::Color>();
            }

            if (node["lit"])
            {
                rhs.m_lit = node["lit"].as<bool>();
            }

            if (node["useVertexColor"])
            {
                rhs.m_useVertexColor = node["useVertexColor"].as<bool>();
            }

            if (node["renderState"])
            {
                return YAML::convert<aiko::RenderState>::decode(node["renderState"], rhs.m_renderState);
            }

            return true;
        }
    };

}
