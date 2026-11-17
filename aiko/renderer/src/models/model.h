#pragma once

#include "assets/types/animation_clip_asset.h"

#include <animation/skeleton_pose.h>
#include <assets/types/skeleton_asset.h>
#include <models/material.h>

#include <memory>
#include <optional>
#include <span>
#include <string_view>
#include <vector>
#include <string>
#include <unordered_map>

namespace aiko
{
    class Mesh;
    struct MeshAsset;
    class RenderResourceManager;

    class Model
    {
    public:
        struct SubMesh
        {
            std::shared_ptr<Mesh> mesh;
            Material material{};
            bool skinned = false;
            std::vector<std::string> morphTargetNames;
        };

        Model() = default;
        ~Model() = default;

        // A Model has one public owner.
        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        Model(Model&&) noexcept = default;
        Model& operator=(Model&&) noexcept = default;

        void addSubMesh(std::shared_ptr<Mesh> mesh, Material material, bool skinned = false, std::vector<std::string> morphTargetNames = {});
        void addSubMesh(const MeshAsset& asset, Material material);

        bool setMorphWeight(std::string_view name, float weight);
        float morphWeight(std::string_view name) const;
        std::vector<float> morphWeightsForSubMesh(size_t subMeshIndex) const;

        bool setJointRotation(std::string_view jointName, float angleRadians, const vec3& axis);

        std::span<const SkeletonJoint> joints() const;
        std::optional<size_t> findJointIndex(std::string_view jointName) const;
        SkeletonPose createBindPose() const;

        bool setPose(const SkeletonPose& pose);
        bool setJointLocalTransform(size_t jointIndex, const mat4& localTransform);
        bool setJointLocalTransform(std::string_view jointName, const mat4& localTransform);

        size_t animationCount() const;
        std::string_view animationName(size_t index) const;

        bool playAnimation(std::string_view clipName, bool loop = true);
        bool playAnimation(size_t index = 0, bool loop = true);
        void updateAnimation(float deltaTime);
        void stopAnimation();

        void unload();

        bool isValid() const;

        const std::vector<SubMesh>& getSubMeshes() const;

    private:
        friend class RenderResourceManager;
        friend class RenderSystem;

        struct State
        {
            std::vector<SubMesh> subMeshes;
            std::optional<SkeletonAsset> skeleton;
            std::vector<mat4> bindSkinMatrices;
            std::vector<AnimationClipAsset> animations;
        };

        void setSkeleton(const SkeletonAsset& skeleton);
        std::span<const mat4> bindSkinMatrices() const;

        void setAnimations(std::vector<AnimationClipAsset> animations);

        // Internal sharing for managed, asynchronously loaded models.
        [[nodiscard]]
        Model share() const;

        // Refreshes data without replacing the existing state.
        void replaceContents(Model&& replacement);
        void clearContents();

        std::shared_ptr<State> m_state = std::make_shared<State>();

        SkeletonPose m_pose;
        std::vector<mat4> m_instanceSkinMatrices;

        std::optional<size_t> m_activeAnimationIndex;

        double m_animationTime = 0.0;
        bool m_animationPlaying = false;
        bool m_animationLoop = true;

        std::unordered_map<std::string, float> m_morphWeights;
    };
}