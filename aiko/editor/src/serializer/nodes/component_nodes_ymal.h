#pragma once

#include <aiko_types.h>

#include <components/transform_component.h>

#include <yaml-cpp/yaml.h>
#include <magic_enum/magic_enum.hpp>

#include "components/camera_component.h"
#include "components/mesh_component.h"

#include "serializer/nodes/core_nodes_ymal.h"
#include "serializer/nodes/render_nodes_ymal.h"

namespace YAML
{

    template<>
    struct convert<aiko::TransforComponent>
    {
        static Node encode(const aiko::TransforComponent& rhs)
        {
            Node node;
            node["position"] = rhs.transform.position;
            node["rotation"] = rhs.transform.rotation;
            node["scale"]    = rhs.transform.scale;
            return node;
        }

        static bool decode(const Node& node, aiko::TransforComponent& rhs)
        {
            if (node.IsMap() == false)
            {
                return false;
            }

            if (node["position"]) rhs.transform.position = node["position"].as<aiko::vec3>();
            if (node["rotation"]) rhs.transform.rotation = node["rotation"].as<aiko::vec3>();
            if (node["scale"])    rhs.transform.scale    = node["scale"].as<aiko::vec3>();
            return true;
        }
    };

    template<>
    struct convert<aiko::CameraComponent>
    {
        static Node encode(const aiko::CameraComponent& rhs)
        {
            Node node;
            node["type"] = magic_enum::enum_name(rhs.getCameraType());
            node["controller"] = magic_enum::enum_name(rhs.getCameraController());
            const aiko::Camera& cam = rhs.getCamera();
            node["fov"]    = cam.getFOV();
            node["dir"]    = cam.getCameraDirection();
            return node;
        }

        static bool decode(const Node& node, aiko::CameraComponent& rhs)
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
    struct convert<aiko::MeshComponent>
    {
        static Node encode(const aiko::MeshComponent& rhs)
        {
            Node node;
            node["meshAsset"] = rhs.getMeshId();
            node["materialAsset"] = rhs.getMaterial();
            node["materialInstance"] = rhs.getMaterialInstance();
            return node;
        }

        static bool decode(const Node& node, aiko::MeshComponent& rhs)
        {
            if (node.IsMap() == false)
            {
                return false;
            }
            AIKO_NOT_IMPLEMENTED;
            return true;
        }
    };

}
