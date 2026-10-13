#pragma once

#include <scene/scene.h>

#include <aiko_types.h>

namespace aiko::editor
{
    class SceneSerializerYAML
    {
    public:
        SceneSerializerYAML() = delete;
        ~SceneSerializerYAML() = delete;

        static bool serializeScene(const Scene& scene, const string& path, string& error);
        static bool deserializeScene(Scene& scene, const string& path, string& error);

    };
}