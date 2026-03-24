#include "scene_serializer_YAML.h"

#include "scene/scene.h"

#include <core/file.h>
#include <logger/logger.h>

#include "ImGuiFileDialogConfig.h"
#include "ImGuiFileDialog.h"
#include <yaml-cpp/yaml.h>

namespace aiko::editor
{

    void SceneSerializerYAML::serializeScene(const Scene& scene, const string& path)
    {
        YAML::Node node;
        AIKO_NOT_IMPLEMENTED;
    }

    void SceneSerializerYAML::deserializeScene(Scene& scene, const string& path)
    {
        const string content = files::readFileContent(path.c_str());
        YAML::Node root = YAML::Load(content);
        AIKO_NOT_IMPLEMENTED;
    }

}
