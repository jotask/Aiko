#include "component_registry.h"

#include <components/transform_component.h>
#include <components/camera_component.h>
#include <components/light_component.h>
#include <components/mesh_component.h>
#include <components/model_component.h>
#include <components/sprite_component.h>

#include "models/component.h"
#include "models/game_object.h"

#include "registry/components_serializer.h"
#include "registry/components_deserializer.h"
#include "registry/components_render.h"

#include "core/editor_context.h"

#include "commands/component/add_component_command.h"
#include "commands/component/remove_component_command.h"

#include <algorithm>
#include <type_traits>
#include <utility>

namespace aiko::editor::component
{

    namespace
    {

        template<class T>
        ComponentEditorEntry makeComponentEntry(string name, string serializedName, bool (*render)(T*), YAML::Node (*serialize)(const T*), bool (*deserialize)(const YAML::Node&, T*), bool addable = true, bool removable = true)
        {
            static_assert(std::is_base_of_v<Component, T>,"Component registry requires a Component type");

            return
            {
                .name = std::move(name),
                .serializedName = std::move(serializedName),
                .type = std::type_index(typeid(T)),

                .addable = addable,
                .removable = removable,

                .has = [](const GameObject& object)
                {
                    return object.hasComponent<T>();
                },

                .add = [](EditorContext& context, GameObject& object)
                {
                    context.commands().execute<AddComponentCommand<T>>(context.sceneSystem(),object);
                },

                .remove = [](EditorContext& context, GameObject& object)
                {
                    context.commands().execute<RemoveComponentCommand<T>>(context.sceneSystem(), object);
                },

                .render = [render](Component& component)
                {
                    return render(static_cast<T*>(&component));
                },

                .serialize = [serialize](const Component& component)
                {
                    return serialize(static_cast<const T*>(&component));
                },

                .deserialize =
                    [deserialize](const YAML::Node& node, GameObject& object)
                    {
                        const bool hadComponent =
                            object.hasComponent<T>();

                        T* component = object.getComponent<T>();

                        if (component == nullptr)
                        {
                            component = object.addComponent<T>();
                        }

                        AIKO_ASSERT(component != nullptr, "Failed to create component during deserialization");

                        if (component == nullptr)
                        {
                            return false;
                        }

                        const bool result = deserialize(node, component);

                        if (result == false && hadComponent == false)
                        {
                            object.removeComponent(component);
                        }

                        return result;
                    },
            };
        }

        const vector<ComponentEditorEntry> s_componentEntries =
        {
            makeComponentEntry<TransformComponent>("Transform", "TransformComponent", drawTransform, serializeTransform, deserializeTransform, false, false),
            makeComponentEntry<CameraComponent>("Camera", "CameraComponent", drawCamera,serializeCamera,deserializeCamera),
            makeComponentEntry<LightComponent>("Light", "LightComponent", drawLight, serializeLight, deserializeLight),
            makeComponentEntry<MeshComponent>("Mesh", "MeshComponent", drawMesh, serializeMesh, deserializeMesh),
            makeComponentEntry<ModelComponent>("Model", "ModelComponent", drawModel, serializeModel, deserializeModel),
            makeComponentEntry<SpriteComponent>("Sprite", "SpriteComponent", drawSprite, serializeSprite, deserializeSprite),
        };

    }

    const vector<ComponentEditorEntry>& componentEntries()
    {
        return s_componentEntries;
    }

    const ComponentEditorEntry* findComponentEntry(const Component& component)
    {
        const std::type_index type = std::type_index(typeid(component));

        const auto it =
            std::find_if(
                s_componentEntries.begin(),
                s_componentEntries.end(),
                [type](const ComponentEditorEntry& entry)
                {
                    return entry.type == type;
                });

        return it != s_componentEntries.end() ? &*it : nullptr;
    }

    const ComponentEditorEntry* findComponentEntryByName(
        std::string_view name)
    {
        const auto it =
            std::find_if(
                s_componentEntries.begin(),
                s_componentEntries.end(),
                [name](const ComponentEditorEntry& entry)
                {
                    return entry.name == name;
                });

        return it != s_componentEntries.end() ? &*it : nullptr;
    }

    const ComponentEditorEntry* findComponentEntryBySerializedName(std::string_view serializedName)
    {
        const auto it =
            std::find_if(
                s_componentEntries.begin(),
                s_componentEntries.end(),
                [serializedName](const ComponentEditorEntry& entry)
                {
                    return  entry.serializedName == serializedName;
                });

        return it != s_componentEntries.end() ? &*it : nullptr;
    }

}