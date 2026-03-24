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

#include "component_serializer.h"
#include "models/game_object.h"

namespace aiko::editor
{

    void SceneSerializerYAML::serializeScene(const Scene& scene, const string& path)
    {
        YAML::Emitter out;
        out << YAML::BeginSeq;
        for (GameObject* obj : scene.getObjects())
        {
            out << YAML::BeginMap;
            out << YAML::Key << "uuid" << YAML::Value << obj->uuid();
            out << YAML::Key << "name" << YAML::Value << obj->getName();
            out << YAML::Key << "components" << YAML::Value;
            out << YAML::BeginSeq;
            for (Component* component : obj->getComponents())
            {
                out << serializer::serializeComponent(component);
            }
            out << YAML::EndSeq;
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;
        std::ofstream file(path, std::ios::out);
        file << out.c_str();
        file.close();

    }

    void SceneSerializerYAML::deserializeScene(Scene& scene, const string& path)
    {
        const string content = files::readFileContent(path.c_str());
        YAML::Node root = YAML::Load(content);
        AIKO_NOT_IMPLEMENTED;
    }

}
