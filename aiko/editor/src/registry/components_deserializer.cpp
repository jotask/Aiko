#include "components_deserializer.h"

#include "components/camera_component.h"
#include "components/light_component.h"
#include "components/mesh_component.h"
#include "components/model_component.h"
#include "components/sprite_component.h"
#include "components/transform_component.h"
#include "serializer/nodes/render_nodes_ymal.h"

#include "models/material.h"
#include "serializer/nodes/core_nodes_ymal.h"

#include <magic_enum/magic_enum.hpp>

namespace aiko::editor::component
{

    bool deserializeTransform(const YAML::Node& node, TransformComponent* component)
    {
        if (!node["position"] || !node["rotation"] || !node["scale"])
        {
            return false;
        }

        component->transform.position = node["position"].as<vec3>();
        component->transform.rotation = node["rotation"].as<vec3>();
        component->transform.scale = node["scale"].as<vec3>();

        return true;
    }

    bool deserializeSprite(const YAML::Node& node, SpriteComponent* component)
    {
        if (node["source"])
        {
            const string source = node["source"].as<string>();

            if (source.empty() == false)
            {
                component->load(source);
            }
        }

        if (node["size"])
        {
            component->setSize(node["size"].as<vec2>());
        }

        if (node["pivot"])
        {
            component->setPivot(node["pivot"].as<vec2>());
        }

        if (node["flipX"])
        {
            component->setFlipX(node["flipX"].as<bool>());
        }

        if (node["flipY"])
        {
            component->setFlipY(node["flipY"].as<bool>());
        }

        if (node["material"])
        {
            if (YAML::convert<Material>::decode(node["material"], component->getMaterial()) == false)
            {
                return false;
            }
        }

        return true;
    }

    bool deserializeMesh(const YAML::Node& node, MeshComponent* component)
    {
        if (!node["primitive"])
        {
            return false;
        }

        const auto primitive = magic_enum::enum_cast<MeshComponent::MeshPrimitive>(node["primitive"].as<string>());

        if (!primitive)
        {
            return false;
        }

        if (*primitive != MeshComponent::MeshPrimitive::None)
        {
            component->loadPrimitive(*primitive);
        }
        else if (node["source"])
        {
            const string source = node["source"].as<string>();

            if (source.empty() == false)
            {
                component->load(source);
            }
        }

        if (node["material"])
        {
            if (YAML::convert<Material>::decode(node["material"], component->getMaterial()) == false)
            {
                return false;
            }
        }

        return true;
    }

    bool deserializeModel(const YAML::Node& node, ModelComponent* component)
    {
        if (!node["source"])
        {
            return false;
        }
        const string source = node["source"].as<string>();
        if (source.empty() == false)
        {
            component->load(source);
        }
        return true;
    }

    bool deserializeLight(const YAML::Node& node, LightComponent* component)
    {
        if (!node["type"] || !node["color"])
        {
            return false;
        }

        const auto type = magic_enum::enum_cast<LightType>(node["type"].as<string>());

        if (!type)
        {
            return false;
        }

        component->type = *type;
        component->color = node["color"].as<Color>();

        if (node["intensity"])
        {
            component->intensity = node["intensity"].as<float>();
        }

        if (node["direction"])
        {
            component->direction = node["direction"].as<vec3>();
        }

        if (node["range"])
        {
            component->range = node["range"].as<float>();
        }

        if (node["innerCos"])
        {
            component->innerCos = node["innerCos"].as<float>();
        }

        if (node["outerCos"])
        {
            component->outerCos = node["outerCos"].as<float>();
        }

        return true;
    }

    bool deserializeCamera(const YAML::Node& node, CameraComponent* component)
    {
        if (!node["controller"] || !node["type"])
        {
            return false;
        }

        const auto controller = magic_enum::enum_cast<camera::CameraController>(node["controller"].as<string>());

        const auto type = magic_enum::enum_cast<Camera::CameraType>(node["type"].as<string>());

        if (!controller || !type)
        {
            return false;
        }

        component->setCameraController(*controller);
        component->setCameraType(*type);

        Camera& camera = component->getCamera();

        if (node["position"])
        {
            camera.position = node["position"].as<vec3>();
        }

        if (node["target"])
        {
            camera.target = node["target"].as<vec3>();
        }

        if (node["fov"])
        {
            camera.m_fov = node["fov"].as<float>();
        }

        if (node["radius"])
        {
            component->radius() = node["radius"].as<float>();
        }

        if (node["speed"])
        {
            component->speed() = node["speed"].as<float>();
        }

        if (node["near"])
        {
            camera.m_near = node["near"].as<float>();
        }

        if (node["far"])
        {
            camera.m_far = node["far"].as<float>();
        }

        if (node["orthoHeight"])
        {
            camera.m_orthoHeight = node["orthoHeight"].as<float>();
        }

        return true;
    }

}