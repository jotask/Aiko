#include "components_serializer.h"

#include "core/imgui_helper.h"

#include "registry/component_registry.h"
#include "serializer/nodes/core_nodes_ymal.h"
#include "serializer/nodes/render_nodes_ymal.h"
#include "serializer/nodes/component_nodes_ymal.h"

namespace aiko::editor
{
    namespace component
    {
        YAML::Node serializeTransform(TransforComponent* c)
        {
            YAML::Node node(YAML::NodeType::Map);
            node["position"] = c->transform.position;
            node["rotation"] = c->transform.rotation;
            node["scale"] = c->transform.scale;
            return node;
        }

        YAML::Node serializeSprite(SpriteComponent* c)
        {
            AIKO_NOT_IMPLEMENTED;
            YAML::Node node;
            return node;
        }

        YAML::Node serializeMesh(MeshComponent* c)
        {
            YAML::Node node;
            node["meshId"] = c->getMeshId();
            node["material"] = c->getMaterial();
            node["materialInstance"] = c->getMaterialInstance();
            return node;
        }

        YAML::Node serializeLight(LightComponent* c)
        {
            AIKO_NOT_IMPLEMENTED;
            YAML::Node node;
            return node;
        }

        YAML::Node serializeCamera(CameraComponent* c)
        {
            YAML::Node node(YAML::NodeType::Map);
            node["controller_type"] = magic_enum::enum_name(c->getCameraController());
            node["view_type"] = magic_enum::enum_name(c->getCameraType());
            node["isMain"] = c->isMain();
            const Camera& camera = c->getCamera();
            node["cam_position"] = camera.position;
            node["cam_target"] = camera.target;
            node["fov"] = camera.getFOV();
            return node;
        }
    }
}