#pragma once

#include <memory>
#include <vector>
#include <optional>
#include <span>

#include "models/material.h"
#include <assets/types/skeleton_asset.h>

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
        };

        Model() = default;
        ~Model() = default;

        // A Model has one public owner.
        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        Model(Model&&) noexcept = default;
        Model& operator=(Model&&) noexcept = default;

        void addSubMesh(std::shared_ptr<Mesh> mesh, Material material, bool skinned = false);
        void addSubMesh(const MeshAsset& asset, Material material);

        void unload();

        [[nodiscard]]
        bool isValid() const;

        [[nodiscard]]
        const std::vector<SubMesh>& getSubMeshes() const;

    private:
        friend class RenderResourceManager;
        friend class RenderSystem;

        struct State
        {
            std::vector<SubMesh> subMeshes;
            std::optional<SkeletonAsset> skeleton;
            std::vector<mat4> bindSkinMatrices;
        };

        void setSkeleton(const SkeletonAsset& skeleton);
        std::span<const mat4> bindSkinMatrices() const;

        // Internal sharing for managed, asynchronously loaded models.
        [[nodiscard]]
        Model share() const;

        // Refreshes data without replacing the existing state.
        void replaceContents(Model&& replacement);
        void clearContents();

        std::shared_ptr<State> m_state = std::make_shared<State>();
    };
}