#pragma once

#include <math/math_vector.h>

#include <cstdint>
#include <string>
#include <vector>

namespace aiko
{
    struct AnimationVectorKey
    {
        double timeSeconds = 0.0;
        vec3 value{};
    };

    struct AnimationRotationKey
    {
        double timeSeconds = 0.0;

        // Quaternion: x, y, z, w.
        vec4 value{0.0f, 0.0f, 0.0f, 1.0f};
    };

    struct AnimationChannel
    {
        uint32_t jointIndex = 0;

        vec3 defaultTranslation{};
        vec4 defaultRotation{0.0f, 0.0f, 0.0f, 1.0f};
        vec3 defaultScale{1.0f, 1.0f, 1.0f};

        // Maps Assimp's local animation space into
        // Aiko's reconstructed skeleton bind space.
        mat4 bindCorrection{1.0f};

        std::vector<AnimationVectorKey> positionKeys;
        std::vector<AnimationRotationKey> rotationKeys;
        std::vector<AnimationVectorKey> scalingKeys;
    };

    struct AnimationClipAsset
    {
        std::string name;
        double durationSeconds = 0.0;

        std::vector<AnimationChannel> channels;
    };
}
