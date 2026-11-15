#include "animation_pose.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <algorithm>
#include <cmath>

namespace aiko
{
    namespace
    {
        template<typename Key, typename Value, typename Interpolate>
        Value sampleKeys(const std::vector<Key>& keys, double timeSeconds, const Value& fallback, Interpolate interpolate)
        {
            if (keys.empty())
            {
                return fallback;
            }

            if (timeSeconds <= keys.front().timeSeconds)
            {
                return keys.front().value;
            }

            if (timeSeconds >= keys.back().timeSeconds)
            {
                return keys.back().value;
            }

            const auto next = std::upper_bound(
                keys.begin(),
                keys.end(),
                timeSeconds,
                [](double time, const Key& key)
                {
                    return time < key.timeSeconds;
                });

            const auto previous = next - 1;

            const double duration = next->timeSeconds - previous->timeSeconds;

            const float alpha = duration > 0.0 ? static_cast<float>((timeSeconds - previous->timeSeconds) / duration) : 0.0f;

            return interpolate(previous->value, next->value, alpha);
        }

        vec3 sampleVector(const std::vector<AnimationVectorKey>& keys, double timeSeconds, const vec3& fallback)
        {
            return sampleKeys(
                keys,
                timeSeconds,
                fallback,
                [](const vec3& a, const vec3& b, float t)
                {
                    return a + (b - a) * t;
                });
        }

        vec4 sampleRotation(const std::vector<AnimationRotationKey>& keys, double timeSeconds, const vec4& fallback)
        {
            return sampleKeys(
                keys,
                timeSeconds,
                fallback,
                [](const vec4& a, const vec4& b, float t)
                {
                    const glm::quat qa(a.w, a.x, a.y, a.z);
                    const glm::quat qb(b.w, b.x, b.y, b.z);

                    const glm::quat result = glm::normalize(glm::slerp(glm::normalize(qa), glm::normalize(qb), t));

                    return vec4{
                        result.x,
                        result.y,
                        result.z,
                        result.w
                    };
                });
        }

        mat4 composeTransform(const vec3& translation, const vec4& rotation, const vec3& scale)
        {
            const glm::quat q = glm::normalize(
                glm::quat(
                    rotation.w,
                    rotation.x,
                    rotation.y,
                    rotation.z));

            const glm::mat4 transform =
                glm::translate(
                    glm::mat4(1.0f),
                    glm::vec3(
                        translation.x,
                        translation.y,
                        translation.z))
                * glm::mat4_cast(q)
                * glm::scale(
                    glm::mat4(1.0f),
                    glm::vec3(scale.x, scale.y, scale.z));

            mat4 result{};

            for (int row = 0; row < 4; ++row)
            {
                for (int col = 0; col < 4; ++col)
                {
                    result(row, col) = transform[col][row];
                }
            }

            return result;
        }
    }

    void AnimationPoseUtils::sample(const AnimationClipAsset& clip, const SkeletonAsset& skeleton, double timeSeconds, SkeletonPose& pose)
    {
        AIKO_ASSERT(std::isfinite(timeSeconds), "Invalid animation sample time");

        // Joints without animation channels stay at bind pose.
        SkeletonPoseUtils::resetToBindPose(skeleton, pose);

        for (const AnimationChannel& channel : clip.channels)
        {
            AIKO_ASSERT(channel.jointIndex < skeleton.joints.size(), "Animation channel has invalid joint index");

            const vec3 position = sampleVector(channel.positionKeys, timeSeconds, channel.defaultTranslation);

            const vec4 rotation = sampleRotation(channel.rotationKeys, timeSeconds, channel.defaultRotation);

            const vec3 scale = sampleVector(channel.scalingKeys, timeSeconds, channel.defaultScale);

            const mat4 animatedLocal = composeTransform(position, rotation, scale);

            pose.localTransforms[channel.jointIndex] = channel.bindCorrection * animatedLocal;
        }
    }
}
