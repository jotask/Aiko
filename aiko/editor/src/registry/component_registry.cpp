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

#include "core/editor_context.h"

#include "commands/component/add_component_command.h"

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
    if (auto* t = dynamic_cast<const cmp*>(c))          \
    {                                                   \
        node["type"] = #cmp;                            \
        node["data"] = fnt(t);                          \
        return true;                                    \
    }                                                   \
    return false;

    const vector<ComponentEditorEntry> s_componentEntries =
    {
        {
            "TransformComponent",
            [](GameObject* go) { return go->hasComponent<TransformComponent>(); },
            [](EditorContext&, GameObject& go) { go.getComponent<TransformComponent>(); },
            [](Component* c) -> bool { RENDER_COMPONENT(TransformComponent, drawTransform) },
            [](const Component* c, YAML::Node& node) -> bool { SERIALIZE_COMPONENT(TransformComponent, serializeTransform) },
            [](const YAML::Node& node, GameObject& obj) -> bool
            {
                if (!node["type"] || node["type"].as<std::string>() != "TransformComponent")
                {
                    return false;
                }

                TransformComponent* t = obj.getComponent<TransformComponent>();

                AIKO_NOT_IMPLEMENTED;
                return true;
            },
        },

         {
            "Camera",
            [](GameObject* go) { return go->hasComponent<CameraComponent>(); },
             [](EditorContext& context, GameObject& go) { context.commands().execute<AddComponentCommand<CameraComponent>>(context.sceneSystem(),go); },
            [](Component* c) -> bool { RENDER_COMPONENT(CameraComponent, drawCamera) },
            [](const Component* c, YAML::Node& node) -> bool { SERIALIZE_COMPONENT(CameraComponent, serializeCamera) },
            [](const YAML::Node& node, GameObject& obj) -> bool
            {
                if (!node["type"] || node["type"].as<std::string>() != "CameraComponent")
                {
                    return false;
                }

                CameraComponent* t = obj.addComponent<CameraComponent>();

                AIKO_NOT_IMPLEMENTED;
                return true;
            },
        },
        {
            "Light",
            [](GameObject* go) { return go->hasComponent<LightComponent>(); },
            [](EditorContext& context, GameObject& go) { context.commands().execute<AddComponentCommand<LightComponent>>(context.sceneSystem(), go); },
            [](Component* c) -> bool { RENDER_COMPONENT(LightComponent, drawLight) },
            [](const Component* c, YAML::Node& node) -> bool { SERIALIZE_COMPONENT(LightComponent, serializeLight) },
            [](const YAML::Node& node, GameObject& obj) -> bool
            {
                AIKO_NOT_IMPLEMENTED;
                return false;
            },
        },
        {
            "Mesh",
            [](GameObject* go) { return go->hasComponent<MeshComponent>(); },
            [](EditorContext& context, GameObject& go) { context.commands().execute<AddComponentCommand<MeshComponent>>( context.sceneSystem(), go); },
            [](Component* c) -> bool { RENDER_COMPONENT(MeshComponent, drawMesh) },
            [](const Component* c, YAML::Node& node) -> bool { SERIALIZE_COMPONENT(MeshComponent, serializeMesh) },
            [](const YAML::Node& node, GameObject& obj) -> bool
            {
                if (!node["type"] || node["type"].as<std::string>() != "MeshComponent")
                {
                    return false;
                }

                MeshComponent* t = obj.addComponent<MeshComponent>();

                AIKO_NOT_IMPLEMENTED;
                return true;
            },
        },
        {
            "Sprite",
            [](GameObject* go) { return go->hasComponent<SpriteComponent>(); },
            [](EditorContext& context, GameObject& go) { context.commands().execute<AddComponentCommand<SpriteComponent>>( context.sceneSystem(), go); },
            [](Component* c) -> bool { RENDER_COMPONENT(SpriteComponent, drawSprite) },
            [](const Component* c, YAML::Node& node) -> bool { SERIALIZE_COMPONENT(SpriteComponent, serializeSprite) },
            [](const YAML::Node& node, GameObject& obj) -> bool
            {
                AIKO_NOT_IMPLEMENTED;
                return false;
            },
        }

    };

}
