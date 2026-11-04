#pragma once

#include <memory>
#include <vector>

#include "models/material.h"

namespace aiko
{
    class Mesh;
    struct MeshAsset;

    class Model
    {
    public:

        struct SubMesh
        {
            std::shared_ptr<Mesh> mesh;
            Material material{};
        };

        Model() = default;
        ~Model() = default;

        // Copy
        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        // Move
        Model(Model&&) noexcept = default;
        Model& operator=(Model&&) noexcept = default;

        void addSubMesh(std::shared_ptr<Mesh> mesh, Material material);
        void addSubMesh(const MeshAsset& asset, Material material);

        void unload();

        [[nodiscard]]
        bool isValid() const;

        [[nodiscard]]
        const std::vector<SubMesh>& getSubMeshes() const
        {
            return m_subMeshes;
        }

    private:

        std::vector<SubMesh> m_subMeshes;
    };
}