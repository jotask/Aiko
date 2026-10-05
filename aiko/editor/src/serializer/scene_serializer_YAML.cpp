#include "scene_serializer_YAML.h"

#include "scene/scene.h"

#include <core/file.h>
#include <logger/logger.h>

#include "ImGuiFileDialogConfig.h"
#include "ImGuiFileDialog.h"

#include <serializer/nodes/core_nodes_ymal.h>
#include <serializer/nodes/component_nodes_ymal.h>
#include <serializer/nodes/node_emitters.h>

#include <yaml-cpp/yaml.h>

#include "registry/components_functionality.h"
#include "models/game_object.h"

namespace aiko::editor
{

    void SceneSerializerYAML::serializeScene(const Scene& scene, const string& path)
    {
        YAML::Emitter out;

        out << YAML::BeginMap;
        out << YAML::Key << "clearColor" << YAML::Value << YAML::convert<Color>::encode(scene.clearColor());
        out << YAML::Key << "ambientLight" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "color" << YAML::Value << YAML::convert<Color>::encode(scene.ambientLight().color);
        out << YAML::Key << "intensity" << YAML::Value << scene.ambientLight().intensity;
        out << YAML::EndMap;

        out << YAML::Key << "activeCamera" << YAML::Value;

        if (const GameObject* camera = scene.getActiveCamera())
        {
            out << camera->uuid();
        }
        else
        {
            out << YAML::Null;
        }

        out << YAML::Key << "objects" << YAML::Value << YAML::BeginSeq;
        for (const GameObject* obj : scene.getObjects())
        {
            out << YAML::BeginMap;
            out << YAML::Key << "uuid" << YAML::Value << obj->uuid();
            out << YAML::Key << "name" << YAML::Value << obj->getName();

            out << YAML::Key << "active" << YAML::Value << obj->isActiveSelf();
            out << YAML::Key << "parent" << YAML::Value;

            if (const GameObject* parent = obj->getParent())
            {
                out << parent->uuid();
            }
            else
            {
                out << YAML::Null;
            }

            out << YAML::Key << "components" << YAML::Value;
            out << YAML::BeginSeq;
            for (const Component* sceneComponent : obj->getComponents())
            {
                YAML::Node componentNode;

                const bool serialized = component::serializeComponent(*sceneComponent, componentNode);

                AIKO_ASSERT(serialized, "Component is not supported by the editor serializer");

                if (serialized)
                {
                    out << componentNode;
                }
            }
            out << YAML::EndSeq;
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream file(path, std::ios::out);
        file << out.c_str();
        file.close();

    }

    void SceneSerializerYAML::deserializeScene(Scene& scene, const string& path)
    {
        const string content = files::readFileContent(path.c_str());

        const YAML::Node root = YAML::Load(content);

        AIKO_ASSERT(root.IsMap(), "Scene root must be a map");

        if (root.IsMap() == false)
        {
            return;
        }

        const YAML::Node objects = root["objects"];

        AIKO_ASSERT(objects && objects.IsSequence(), "Scene objects must be a sequence");

        if (!objects || objects.IsSequence() == false)
        {
            return;
        }

        scene.clear();

        if (root["clearColor"])
        {
            scene.clearColor() = root["clearColor"].as<Color>();
        }

        if (const YAML::Node ambient = root["ambientLight"])
        {
            if (ambient["color"])
            {
                scene.ambientLight().color = ambient["color"].as<Color>();
            }

            if (ambient["intensity"])
            {
                scene.ambientLight().intensity = ambient["intensity"].as<float>();
            }
        }

        for (const YAML::Node& objectNode : objects)
        {
            if (!objectNode["uuid"] || !objectNode["name"])
            {
                continue;
            }

            const uuid::Uuid id = objectNode["uuid"].as<uuid::Uuid>();
            const string name = objectNode["name"].as<string>();

            GameObject* object = scene.create(id, name);

            AIKO_ASSERT(object != nullptr, "Failed to create GameObject while loading scene");

            if (object == nullptr)
            {
                continue;
            }

            if (objectNode["active"])
            {
                object->setActive(objectNode["active"].as<bool>());
            }

            const YAML::Node components = objectNode["components"];

            if (!components || components.IsSequence() == false)
            {
                continue;
            }

            for (const YAML::Node& componentNode : components)
            {
                component::deserializeComponent(componentNode, *object);
            }
        }

        for (const YAML::Node& objectNode : objects)
        {
            if (!objectNode["uuid"])
            {
                continue;
            }

            const YAML::Node parentNode = objectNode["parent"];

            if (!parentNode || parentNode.IsNull())
            {
                continue;
            }

            GameObject* object = scene.find(objectNode["uuid"].as<uuid::Uuid>());
            GameObject* parent = scene.find(parentNode.as<uuid::Uuid>());

            AIKO_ASSERT(object != nullptr, "Failed to resolve GameObject while restoring hierarchy");
            AIKO_ASSERT(parent != nullptr, "Failed to resolve parent GameObject while restoring hierarchy");

            if (object != nullptr && parent != nullptr)
            {
                object->transform().setParent(&parent->transform());
            }
        }

        const YAML::Node activeCamera = root["activeCamera"];

        if (activeCamera && !activeCamera.IsNull())
        {
            GameObject* camera = scene.find(activeCamera.as<uuid::Uuid>());
            AIKO_ASSERT(camera != nullptr, "Failed to restore active camera");
            if (camera != nullptr)
            {
                scene.setActiveCamera(camera);
            }
        }
    }

}
