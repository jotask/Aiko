#include "components_deserializer.h"

#include "components/model_component.h"

#include <aiko_types.h>

namespace aiko::editor::component
{

    bool deserializeTransform(const YAML::Node&, TransformComponent*)
    {
        AIKO_NOT_IMPLEMENTED;
        return false;
    }

    bool deserializeSprite(const YAML::Node&, SpriteComponent*)
    {
        AIKO_NOT_IMPLEMENTED;
        return false;
    }

    bool deserializeMesh(const YAML::Node&, MeshComponent*)
    {
        AIKO_NOT_IMPLEMENTED;
        return false;
    }

    bool deserializeModel(const YAML::Node& node, ModelComponent* component)
    {
        if (!node["source"])
        {
            return false;
        }
        const string source = node["source"].as<string>();
        if (source.empty())
        {
            return false;
        }
        component->load(source);
        return true;
    }

    bool deserializeLight(const YAML::Node&, LightComponent*)
    {
        AIKO_NOT_IMPLEMENTED;
        return false;
    }

    bool deserializeCamera(const YAML::Node&, CameraComponent*)
    {
        AIKO_NOT_IMPLEMENTED;
        return false;
    }

}