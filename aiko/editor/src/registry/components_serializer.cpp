#include "components_serializer.h"

#include "core/imgui_helper.h"
#include "components/model_component.h"
#include "registry/component_registry.h"
#include "serializer/nodes/core_nodes_ymal.h"
#include "serializer/nodes/render_nodes_ymal.h"
#include "serializer/nodes/component_nodes_ymal.h"

namespace aiko::editor
{
    namespace component
    {
        YAML::Node serializeTransform(const TransformComponent* component)
        {
            YAML::Node node(YAML::NodeType::Map);
            node["position"] = component->transform.position;
            node["rotation"] = component->transform.rotation;
            node["scale"] = component->transform.scale;
            return node;
        }

        YAML::Node serializeSprite(const SpriteComponent* component)
        {
            YAML::Node node(YAML::NodeType::Map);

            node["source"] = component->getAssetSource();
            node["size"] = component->getSize();
            node["pivot"] = component->getPivot();
            node["flipX"] = component->getFlipX();
            node["flipY"] = component->getFlipY();
            node["material"] = component->getMaterial();
            return node;
        }

        YAML::Node serializeMesh(const MeshComponent* component)
        {
            YAML::Node node(YAML::NodeType::Map);
            node["primitive"] = string(magic_enum::enum_name(component->getPrimitive()));
            node["source"] = component->getAssetSource();
            node["material"] = component->getMaterial();
            return node;
        }

        YAML::Node serializeModel(const ModelComponent* component)
        {
            YAML::Node node(YAML::NodeType::Map);
            node["source"] = component->getAssetSource();
            return node;
        }

        YAML::Node serializeLight(const LightComponent* component)
        {
            YAML::Node node(YAML::NodeType::Map);
            node["type"] = string(magic_enum::enum_name(component->type));
            node["color"] = component->color;
            node["intensity"] = component->intensity;
            node["direction"] = component->direction;
            node["range"] = component->range;
            node["innerCos"] = component->innerCos;
            node["outerCos"] = component->outerCos;
            return node;
        }

        YAML::Node serializeCamera(const CameraComponent* component)
        {
            YAML::Node node(YAML::NodeType::Map);

            node["controller"] = string(magic_enum::enum_name(component->getCameraController()));
            node["type"] = string(magic_enum::enum_name(component->getCameraType()));

            const Camera& camera = component->getCamera();

            node["near"] = camera.m_near;
            node["far"] = camera.m_far;
            node["orthoHeight"] = camera.m_orthoHeight;
            node["position"] = camera.position;
            node["target"] = camera.target;
            node["fov"] = camera.getFOV();
            node["radius"] = component->radius();
            node["speed"] = component->speed();

            return node;
        }
    }
}