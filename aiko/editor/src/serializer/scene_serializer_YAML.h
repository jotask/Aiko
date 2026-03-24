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

        static void serializeScene(const Scene& scene, const string& path);
        static void deserializeScene(Scene& scene, const string& path);

    };
}