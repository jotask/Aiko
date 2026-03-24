#include "component_registry.h"

#include <components/transform_component.h>
#include <components/camera_component.h>
#include <components/light_component.h>
#include <components/mesh_component.h>
#include <components/sprite_component.h>

#include "registry/components_serializer.h"
#include "registry/components_deserializer.h"
#include "registry/components_functionality.h"
#include "registry/components_render.h"

namespace aiko::editor::component
{

    #define RENDER_COMPONENT(cmp, fnt)                  \
    if (auto* t = dynamic_cast<cmp*>(c))                \
    {                                                   \
        (fnt)(t);                                       \
        return true;                                    \
    }                                                   \
    return false;

    #define SERIALIZE_COMPONENT(cmp, fnt)               \
    if (auto* t = dynamic_cast<cmp*>(c))                \
    {                                                   \
        node["type"] = #cmp;                            \
        node["data"] = fnt(t);                          \
        return true;                                    \
    }                                                   \
    return false;

    const std::vector<ComponentEditorEntry> s_componentEntries =
    {
        {
            "TransformComponent",
            [](GameObject* go) { return go->hasComponent<TransforComponent>(); },
            [](GameObject* go) { go->addComponent<TransforComponent>(); },
            [](GameObject* go) { return go->removeComponent<TransforComponent>(); },
            [](Component* c) -> bool { RENDER_COMPONENT(TransforComponent, drawTransform) },
            [](Component* c, YAML::Node& node) -> bool { SERIALIZE_COMPONENT(TransforComponent, serializeTransform) },
            [](const YAML::Node& node, GameObject& obj) -> bool
            {
                if (!node["type"] || node["type"].as<std::string>() != "TransformComponent")
                {
                    return false;
                }

                AikoPtr<TransforComponent> t = obj.addComponent<TransforComponent>();

                AIKO_NOT_IMPLEMENTED;
                return true;
            },
        },

         {
            "Camera",
            [](GameObject* go) { return go->hasComponent<CameraComponent>(); },
            [](GameObject* go) { go->addComponent<CameraComponent>(); },
            [](GameObject* go) { return go->removeComponent<CameraComponent>(); },
            [](Component* c) -> bool { RENDER_COMPONENT(CameraComponent, drawCamera) },
            [](Component* c, YAML::Node& node) -> bool { SERIALIZE_COMPONENT(CameraComponent, serializeCamera) },
            [](const YAML::Node& node, GameObject& obj) -> bool
            {
                if (!node["type"] || node["type"].as<std::string>() != "CameraComponent")
                {
                    return false;
                }

                AikoPtr<CameraComponent> t = obj.addComponent<CameraComponent>();

                AIKO_NOT_IMPLEMENTED;
                return true;
            },
        },
        {
            "Light",
            [](GameObject* go) { return go->hasComponent<LightComponent>(); },
            [](GameObject* go) { go->addComponent<LightComponent>(); },
            [](GameObject* go) { return go->removeComponent<LightComponent>(); },
            [](Component* c) -> bool { RENDER_COMPONENT(LightComponent, drawLight) },
            [](Component* c, YAML::Node& node) -> bool { SERIALIZE_COMPONENT(LightComponent, serializeLight) },
            [](const YAML::Node& node, GameObject& obj) -> bool
            {
                AIKO_NOT_IMPLEMENTED;
                return false;
            },
        },
        {
            "Mesh",
            [](GameObject* go) { return go->hasComponent<MeshComponent>(); },
            [](GameObject* go) { go->addComponent<MeshComponent>(); },
            [](GameObject* go) { return go->removeComponent<MeshComponent>(); },
            [](Component* c) -> bool { RENDER_COMPONENT(MeshComponent, drawMesh) },
            [](Component* c, YAML::Node& node) -> bool { SERIALIZE_COMPONENT(MeshComponent, serializeMesh) },
            [](const YAML::Node& node, GameObject& obj) -> bool
            {
                if (!node["type"] || node["type"].as<std::string>() != "MeshComponent")
                {
                    return false;
                }

                AikoPtr<MeshComponent> t = obj.addComponent<MeshComponent>();

                AIKO_NOT_IMPLEMENTED;
                return true;
            },
        },
        {
            "Sprite",
            [](GameObject* go) { return go->hasComponent<SpriteComponent>(); },
            [](GameObject* go) { go->addComponent<SpriteComponent>(); },
            [](GameObject* go) { return go->removeComponent<SpriteComponent>(); },
            [](Component* c) -> bool { RENDER_COMPONENT(SpriteComponent, drawSprite) },
            [](Component* c, YAML::Node& node) -> bool { SERIALIZE_COMPONENT(SpriteComponent, serializeSprite) },
            [](const YAML::Node& node, GameObject& obj) -> bool
            {
                AIKO_NOT_IMPLEMENTED;
                return false;
            },
        }

    };

}
