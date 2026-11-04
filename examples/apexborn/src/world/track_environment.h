#pragma once

#include "core/transform.h"
#include "models/material.h"
#include "models/mesh.h"

#include <math/math_vector.h>

#include <models/model.h>
#include <types/draw_types.h>

#include <vector>
#include <cstdint>

namespace aiko
{
    class AssetContext;
    class RenderContext;
}

namespace ab
{
    class Track;

    class TrackEnvironment final
    {
    public:
        void init(aiko::AssetContext& assets);

        bool generate(const Track& track, const aiko::vec2& size, uint32_t seed, uint32_t resolution = 65);

        void render(aiko::RenderContext& context) const;

    private:
        aiko::Mesh m_groundMesh;
        aiko::Material m_groundMaterial;

        enum class EnvironmentModelType
        {
            FinishBanner,
            Tree,
            Rock,
            Bush,
            Decoration
        };

        struct EnvironmentInstance
        {
            size_t modelIdx;
            aiko::Transform transform;
        };

        std::vector<aiko::Model> m_environmentModels;
        std::vector<EnvironmentInstance> m_modelsInstances;
        std::vector<std::vector<aiko::InstanceData>> m_treeInstancesByModel;

        std::vector<size_t> m_treeModelIndices;
        size_t m_bannerModelIdx = 0;

    };

}
