#include "track_environment.h"

#include "assets/types/mesh_asset.h"
#include "core/transform.h"
#include "layers/contexts/asset_context.h"
#include "layers/contexts/render_context.h"
#include "models/mesh_factory.h"
#include "track.h"

#include <cmath>

namespace ab
{
    namespace
    {
        constexpr float GroundHeight = -0.12f;
        constexpr aiko::Color GroundColor = { 0.23f, 0.35f, 0.22f};
    }

    void TrackEnvironment::init(aiko::AssetContext& assets)
    {
        m_groundMaterial.m_shaderId = assets.loadShader("model");
        m_groundMaterial.m_lit = true;
        m_groundMaterial.m_useVertexColor = false;
        m_groundMaterial.m_baseColor = GroundColor;

        struct ModelDefinition
        {
            const char* filename;
            EnvironmentModelType type;
        };

        const std::vector<ModelDefinition> modelsToLoad = {
            {"banner-hanging-high.fbx", EnvironmentModelType::FinishBanner},
            {"tree.fbx",                EnvironmentModelType::Tree},
            {"tree-large.fbx",          EnvironmentModelType::Tree},
            {"tree-tall.fbx",           EnvironmentModelType::Tree},
            {"tree-tall-large.fbx",     EnvironmentModelType::Tree}
        };

        m_environmentModels.clear();
        m_modelsInstances.clear();
        m_treeModelIndices.clear();

        for (const auto& definition : modelsToLoad)
        {
            const size_t modelIdx = m_environmentModels.size();

            m_environmentModels.emplace_back(assets.loadModel(definition.filename));

            switch (definition.type)
            {
                case EnvironmentModelType::FinishBanner:
                    m_bannerModelIdx = modelIdx;
                    break;

                case EnvironmentModelType::Tree:
                    m_treeModelIndices.push_back(modelIdx);
                    break;

                default:
                    break;
            }
        }

    }

    bool TrackEnvironment::generate(const Track& track, const aiko::vec2& size, uint32_t seed, uint32_t resolution)
    {
        if (!aiko::math::isfinite(size.x) || !aiko::math::isfinite(size.y) || size.x <= 0.0f || size.y <= 0.0f || resolution < 2 || resolution > 256)
        {
            return false;
        }

        std::mt19937 rng(seed);

        auto asset = aiko::mesh::factory::generateMeshPlane(size.x, size.y, static_cast<int>(resolution), static_cast<int>(resolution));

        for (auto& vertex : asset.m_vertices)
        {
            // TODO Generate 3D noise for more visual appealing terrain
            vertex.y = GroundHeight;
        }

        aiko::mesh::factory::recalculateNormals(asset);

        m_groundMesh.upload(asset);

        // Decoration
        m_modelsInstances.clear();
        {

            // Finish line
            {
                const auto finishLine = track.getFinishLine();
                aiko::Transform transform;
                transform.position = finishLine.position;
                const float heading = std::atan2(finishLine.forward.z, finishLine.forward.x);
                constexpr float BannerYawOffset = 90.0f;
                transform.rotation = { 0.0f, -aiko::math::degrees(heading) + BannerYawOffset, 0.0f };
                transform.scale= { 1.75f };
                m_modelsInstances.emplace_back(m_bannerModelIdx, transform);
            }

            // Trees
            {
                constexpr size_t NumberOfTrees = 100;
                constexpr size_t MaxAttempts = NumberOfTrees * 20;

                constexpr float TreeRadius = 0.5f;
                constexpr float RoadClearance = 0.5f;
                constexpr float TreeSpacing = 0.25f;

                std::uniform_real_distribution<float> distributionX( -size.x * 0.5f, size.x * 0.5f);
                std::uniform_real_distribution<float> distributionZ( -size.y * 0.5f, size.y * 0.5f);

                std::uniform_real_distribution<float> rotationDistribution( 0.0f, 360.0f );
                std::uniform_real_distribution<float> scaleDistribution( 0.85f, 1.15f );

                AIKO_ASSERT(!m_treeModelIndices.empty(), "No tree models loaded");
                std::uniform_int_distribution<size_t> treeDistribution( 0, m_treeModelIndices.size() - 1 );

                struct TreePosition
                {
                    aiko::vec2 position;
                    float radius;
                };

                std::vector<TreePosition> treePositions;
                treePositions.reserve(NumberOfTrees);

                size_t attempts = 0;

                while (treePositions.size() < NumberOfTrees && attempts < MaxAttempts)
                {
                    ++attempts;

                    const float x = distributionX(rng);
                    const float z = distributionZ(rng);

                    const aiko::vec3 point = {x, GroundHeight, z};

                    const float scaleVariation = scaleDistribution(rng);
                    const float radius = TreeRadius * scaleVariation;

                    // Check distance from road
                    const auto projection = track.projectPoint(point);

                    const float dx = point.x - projection.closestToCenterLine.x;
                    const float dz = point.z - projection.closestToCenterLine.z;

                    const float distanceFromRoadCenter = std::sqrt( dx * dx + dz * dz );

                    const float minimumRoadDistance = track.getTrackWidth() * 0.5f + radius + RoadClearance;

                    if (distanceFromRoadCenter < minimumRoadDistance)
                    {
                        continue;
                    }

                    // Check distance from other trees
                    bool overlaps = false;

                    for (const auto& existing : treePositions)
                    {
                        const float tx = x - existing.position.x;
                        const float tz = z - existing.position.y;

                        const float distanceSquared = tx * tx + tz * tz;

                        const float minimumDistance = radius + existing.radius + TreeSpacing;

                        if (distanceSquared < minimumDistance * minimumDistance)
                        {
                            overlaps = true;
                            break;
                        }
                    }

                    if (overlaps)
                    {
                        continue;
                    }

                    // Accept tree
                    treePositions.push_back({ {x, z}, radius});

                    auto& instance = m_modelsInstances.emplace_back();

                    instance.modelIdx = m_treeModelIndices[treeDistribution(rng)];
                    instance.transform.position = point;
                    instance.transform.rotation = { 0.0f, rotationDistribution(rng), 0.0f };

                    const float treeScale = 1.0f * scaleVariation;
                    instance.transform.scale = {treeScale};
                }
            }

        }

        return true;
    }

    void TrackEnvironment::render(aiko::RenderContext& context) const
    {
        // Ground
        if (m_groundMesh.isValid())
        {
            aiko::Transform transform{};
            context.drawMesh(transform, m_groundMesh, m_groundMaterial);
        }

        // Environment decorations
        for (const auto& instance : m_modelsInstances)
        {
            AIKO_ASSERT(instance.modelIdx < m_environmentModels.size(), "Invalid environment model index");
            const aiko::Model& model = m_environmentModels[instance.modelIdx];
            context.drawModel(instance.transform, model);
        }
    }
}
