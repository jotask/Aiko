#include "scene_serializer_YAML.h"

#include "scene/scene.h"

#include <exception>
#include <filesystem>
#include <fstream>
#include <components/camera_component.h>
#include <unordered_map>
#include <unordered_set>

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

    bool SceneSerializerYAML::serializeScene(const Scene& scene, const string& path, string& error)
    {

        error.clear();
        std::filesystem::path temporaryPath;

        const auto fail = [&](string message)
        {
            error = std::move(message);

            if (temporaryPath.empty() == false)
            {
                std::error_code cleanupError;
                std::filesystem::remove(temporaryPath, cleanupError);
            }

            return false;
        };

        try
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

                    if (!component::serializeComponent(*sceneComponent, componentNode))
                    {
                        return fail("Cannot save an unsupported component on object: " + obj->getName());
                    }

                    out << componentNode;

                }
                out << YAML::EndSeq;
                out << YAML::EndMap;
            }
            out << YAML::EndSeq;
            out << YAML::EndMap;

            if (!out.good())
            {
                return fail("Could not encode scene YAML: " + out.GetLastError());
            }

            if (path.empty())
            {
                return fail("The scene save path is empty.");
            }

            const std::filesystem::path destination(path);

            temporaryPath = destination;
            temporaryPath += ".tmp-" + uuid::Uuid{}.get();

            bool writeSucceeded = false;
            bool closeSucceeded = false;

            {
                std::ofstream file(temporaryPath,std::ios::out | std::ios::binary | std::ios::trunc);

                if (file.is_open() == false)
                {
                    return fail("Could not open a temporary scene file for: " + path);
                }

                file.write(out.c_str(), static_cast<std::streamsize>(out.size()));
                file.flush();
                writeSucceeded = file.good();

                file.close();
                closeSucceeded = !file.fail();
            }

            if (!writeSucceeded || !closeSucceeded)
            {
                return fail("Could not finish writing the scene file: " + path);
            }

            std::error_code renameError;
            std::filesystem::rename(temporaryPath, destination, renameError);

            if (renameError)
            {
                return fail("Could not replace the scene file: " + path + "\n" + renameError.message());
            }

            temporaryPath.clear();
            return true;

        }
        catch (const std::exception& exception)
        {
            return fail("Could not save the scene: " + path + "\n" + exception.what());
        }

    }

    bool SceneSerializerYAML::deserializeScene(Scene& scene, const string& path, string& error)
    {
        error.clear();

        const auto fail = [&](string message)
        {
            error = std::move(message);
            return false;
        };

        try
        {
            std::ifstream file(path, std::ios::in | std::ios::binary);

            if (!file.is_open())
            {
                return fail("Could not open the scene file: " + path);
            }

            string content;
            string line;

            while (std::getline(file, line))
            {
                content += line;
                content += '\n';
            }

            if (file.bad() || !file.eof())
            {
                return fail("Could not read the scene file: " + path);
            }

            const YAML::Node root = YAML::Load(content);

            if (!root.IsMap())
            {
                return fail("Scene root must be a map.");
            }

            const YAML::Node objects = root["objects"];

            if (!objects || !objects.IsSequence())
            {
                return fail("Scene objects must be a sequence.");
            }

            Scene replacement;

            if (root["clearColor"])
            {
                replacement.clearColor() = root["clearColor"].as<Color>();
            }

            if (const YAML::Node ambient = root["ambientLight"])
            {
                if (!ambient.IsMap())
                {
                    return fail("Scene ambientLight must be a map.");
                }

                if (ambient["color"])
                {
                    replacement.ambientLight().color = ambient["color"].as<Color>();
                }

                if (ambient["intensity"])
                {
                    replacement.ambientLight().intensity = ambient["intensity"].as<float>();
                }
            }

            std::unordered_map<uuid::Uuid, uuid::Uuid> parents;

            for (const YAML::Node& objectNode : objects)
            {
                if (!objectNode.IsMap() || !objectNode["uuid"] || !objectNode["name"])
                {
                    return fail("Every scene object requires a UUID and name.");
                }

                const uuid::Uuid id = objectNode["uuid"].as<uuid::Uuid>();

                if (id.get().empty())
                {
                    return fail("Scene object UUID must not be empty.");
                }

                if (replacement.find(id) != nullptr)
                {
                    return fail("Duplicate scene object UUID: " + id.get());
                }

                GameObject* object = replacement.create(id, objectNode["name"].as<string>());

                if (objectNode["active"])
                {
                    object->setActive(objectNode["active"].as<bool>());
                }

                const YAML::Node components = objectNode["components"];

                if (!components || !components.IsSequence())
                {
                    return fail("Object components must be a sequence: " + id.get());
                }

                std::unordered_set<string> componentTypes;

                for (const YAML::Node& componentNode : components)
                {
                    if (!componentNode.IsMap() ||
                        !componentNode["type"] ||
                        !componentNode["data"] ||
                        !componentNode["data"].IsMap())
                    {
                        return fail("Invalid component entry on object: " + id.get());
                    }

                    const string type = componentNode["type"].as<string>();

                    if (!componentTypes.insert(type).second)
                    {
                        return fail("Duplicate component " + type + " on object: " + id.get());
                    }

                    if (!component::deserializeComponent(componentNode, *object))
                    {
                        return fail("Unsupported or invalid component " + type + " on object: " + id.get());
                    }
                }

                const YAML::Node parentNode = objectNode["parent"];

                if (parentNode && !parentNode.IsNull())
                {
                    parents.emplace(id, parentNode.as<uuid::Uuid>());
                }
            }

            // Validate references before attaching any hierarchy.
            for (const auto& [childId, parentId] : parents)
            {
                if (replacement.find(parentId) == nullptr)
                {
                    return fail("Missing parent " + parentId.get() + " for object: " + childId.get());
                }
            }

            // A valid ancestor chain cannot contain more edges than objects.
            for (const auto& [childId, parentId] : parents)
            {
                uuid::Uuid current = childId;
                size_t remaining = objects.size();

                while (true)
                {
                    const auto it = parents.find(current);

                    if (it == parents.end())
                    {
                        break;
                    }

                    if (remaining == 0)
                    {
                        return fail("Hierarchy cycle involving object: " + childId.get());
                    }

                    --remaining;
                    current = it->second;
                }
            }

            for (const auto& [childId, parentId] : parents)
            {
                GameObject* child = replacement.find(childId);
                GameObject* parent = replacement.find(parentId);

                child->transform().setParent(&parent->transform());
            }

            const YAML::Node activeCamera = root["activeCamera"];

            if (activeCamera && !activeCamera.IsNull())
            {
                const uuid::Uuid id = activeCamera.as<uuid::Uuid>();
                GameObject* camera = replacement.find(id);

                if (camera == nullptr || camera->getComponent<CameraComponent>() == nullptr)
                {
                    return fail("Active camera must reference an object with a CameraComponent: " + id.get());
                }

                replacement.setActiveCamera(camera);
            }

            // No live scene changes occur before this point.
            scene.swap(replacement);
            return true;
        }
        catch (const std::exception& exception)
        {
            return fail("Could not load the scene: " + path + "\n" + exception.what());
        }
    }

}
