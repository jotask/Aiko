#pragma once

#include <yaml-cpp/yaml.h>

#include <aiko_types.h>
#include <core/uuid.h>
#include <math/math_vector.h>

namespace YAML
{
    inline Emitter& operator<<(Emitter& out, const aiko::uuid::Uuid& uuid)
    {
        out << uuid.get();
        return out;
    }

    inline Emitter& operator<<(Emitter& out, const aiko::vec3& v)
    {
        out << Flow;
        out << BeginSeq << v.x << v.y << v.z << EndSeq;
        return out;
    }
}