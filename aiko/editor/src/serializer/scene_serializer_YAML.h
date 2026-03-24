#pragma once

#include <scene/scene.h>

#include <aiko_types.h>

namespace aiko::editor
{
    class SceneSerializerYAML
    {
    public:
        SceneSerializerYAML() = default;
        ~SceneSerializerYAML() = default;

        void serializeScene(const Scene& scene, const string& path);
        void deserializeScene(Scene& scene, const string& path);

    };
}