#include "sph_gpu_simulation.h"

#include "assets/types/shader_asset.h"
#include "systems/asset_system.h"
#include "systems/render_system.h"

#include <cmath>

namespace sph
{

    namespace
    {

        struct SPHHashPushConstants
        {
            float smoothingRadius = 0.0f;
            uint32_t particleCount = 0;
            uint32_t sortParticleCount = 0;
            uint32_t gridWidth = 0;

            uint32_t gridHeight = 0;
            uint32_t padding0 = 0;
            uint32_t padding1 = 0;
            uint32_t padding2 = 0;

            alignas(16) aiko::vec4 boundsMin = {};
        };

        static_assert(sizeof(SPHHashPushConstants) == 48);

        struct SPHSortPushConstants
        {
            uint32_t particleCount = 0;
            uint32_t k = 0;
            uint32_t j = 0;
            uint32_t padding = 0;
        };

        static_assert(sizeof(SPHSortPushConstants) == 16);

        struct SPHClearCellsPushConstants
        {
            uint32_t cellCount = 0;
        };

        struct SPHCellRangePushConstants
        {
            uint32_t particleCount = 0;
            uint32_t cellCount = 0;
            uint32_t padding0 = 0;
            uint32_t padding1 = 0;
        };

        struct SPHViscosityPushConstants
        {
            float dt = 0.0f;
            float smoothingRadius = 0.0f;
            float sigma = 0.0f;
            float beta = 0.0f;

            uint32_t particleCount = 0;
            uint32_t gridWidth = 0;
            uint32_t gridHeight = 0;
            uint32_t padding = 0;

            alignas(16) aiko::vec4 boundsMin = {};
        };

        static_assert(sizeof(SPHViscosityPushConstants) == 48);

        struct SPHApplyViscosityPushConstants
        {
            uint32_t particleCount = 0;
        };

        static_assert(sizeof(SPHApplyViscosityPushConstants) == 4);

        struct SPHDensityPushConstants
        {
            float smoothingRadius = 0.0f;
            float restDensity = 0.0f;
            float pressureStiffness = 0.0f;
            float nearPressureStiffness = 0.0f;

            uint32_t particleCount = 0;
            uint32_t gridWidth = 0;
            uint32_t gridHeight = 0;
            uint32_t padding = 0;

            alignas(16) aiko::vec4 boundsMin = {};
        };

        static_assert(sizeof(SPHDensityPushConstants) == 48);

        struct SPHRelaxationPushConstants
        {
            float dt = 0.0f;
            float smoothingRadius = 0.0f;
            uint32_t particleCount = 0;
            uint32_t gridWidth = 0;

            uint32_t gridHeight = 0;
            uint32_t padding0 = 0;
            uint32_t padding1 = 0;
            uint32_t padding2 = 0;

            alignas(16) aiko::vec4 boundsMin = {};
        };

        static_assert(sizeof(SPHRelaxationPushConstants) == 48);

        struct SPHApplyPositionDeltaPushConstants
        {
            uint32_t particleCount = 0;
        };

        static_assert(sizeof(SPHApplyPositionDeltaPushConstants) == 4);

        struct SPHGravityPushConstants
        {
            float dt = 0.0f;
            float gravity = 0.0f;
            uint32_t particleCount = 0;
            uint32_t padding = 0;

            alignas(16) aiko::vec4 gravityDirection = {};
        };

        static_assert(sizeof(SPHGravityPushConstants) == 32);

        struct SPHPredictPushConstants
        {
            float dt = 0.0f;
            uint32_t particleCount = 0;
            uint32_t padding0 = 0;
            uint32_t padding1 = 0;
        };

        static_assert(sizeof(SPHPredictPushConstants) == 16);

        struct SPHBoundaryPushConstants
        {
            float particleRadius = 0.0f;
            uint32_t particleCount = 0;
            uint32_t padding0 = 0;
            uint32_t padding1 = 0;

            alignas(16) aiko::vec4 boundsPosition = {};
            alignas(16) aiko::vec4 boundsSize = {};
        };

        static_assert(sizeof(SPHBoundaryPushConstants) == 48);

        struct SPHComputeVelocityPushConstants
        {
            float dt = 0.0f;
            uint32_t particleCount = 0;
            uint32_t padding0 = 0;
            uint32_t padding1 = 0;
        };

        static_assert(sizeof(SPHComputeVelocityPushConstants) == 16);

        static constexpr uint32_t EmptySpringKey = 0xFFFFFFFFu;
        static constexpr uint32_t MaxPackedParticleCount = 1u << 16;

        struct GpuSpring
        {
            uint32_t pairKey = EmptySpringKey;
            float restLength = 0.0f;
            uint32_t enabled = 0;
            uint32_t padding = 0;
        };

        static_assert(sizeof(GpuSpring) == 16);
        static_assert((MaxSprings & (MaxSprings - 1)) == 0);

        struct SPHGenerateSpringsPushConstants
        {
            float smoothingRadius = 0.0f;
            float dt = 0.0f;
            float gamma = 0.0f;
            float plasticity = 0.0f;

            uint32_t particleCount = 0;
            uint32_t gridWidth = 0;
            uint32_t gridHeight = 0;
            uint32_t maxSprings = 0;

            alignas(16) aiko::vec4 boundsMin = {};
        };

        static_assert(sizeof(SPHGenerateSpringsPushConstants) == 48);

        struct SPHMigrateSpringsPushConstants
        {
            float dt = 0.0f;
            float gamma = 0.0f;
            float plasticity = 0.0f;
            float smoothingRadius = 0.0f;

            uint32_t maxSprings = 0;
            uint32_t padding0 = 0;
            uint32_t padding1 = 0;
            uint32_t padding2 = 0;
        };

        static_assert(sizeof(SPHMigrateSpringsPushConstants) == 32);

        struct SPHSpringDisplacementPushConstants
        {
            float dt = 0.0f;
            float smoothingRadius = 0.0f;
            float springStiffness = 0.0f;
            uint32_t particleCount = 0;

            uint32_t maxSprings = 0;
            uint32_t gridWidth = 0;
            uint32_t gridHeight = 0;
            uint32_t padding = 0;

            alignas(16) aiko::vec4 boundsMin = {};
        };

        static_assert(sizeof(SPHSpringDisplacementPushConstants) == 48);

        struct GpuShapeEdge
        {
            alignas(16) aiko::vec4 a = {};
            alignas(16) aiko::vec4 b = {};
        };

        static_assert(sizeof(GpuShapeEdge) == 32);

        struct GpuShape
        {
            uint32_t edgeStart = 0;
            uint32_t edgeCount = 0;
            uint32_t padding0 = 0;
            uint32_t padding1 = 0;
        };

        static_assert(sizeof(GpuShape) == 16);

        struct SPHShapeCollisionPushConstants
        {
            float particleRadius = 0.0f;
            uint32_t particleCount = 0;
            uint32_t shapeCount = 0;
            uint32_t padding = 0;
        };

        static_assert(sizeof(SPHShapeCollisionPushConstants) == 16);

        struct SPHClearSpringsPushConstants
        {
            uint32_t maxSprings = 0;
        };

        static_assert(sizeof(SPHClearSpringsPushConstants) == 4);

        struct SPHStickinessPushConstants
        {
            float dt = 0.0f;
            float maxStickiness = 0.0f;
            float kStick = 0.0f;
            uint32_t particleCount = 0;

            uint32_t shapeCount = 0;
            uint32_t padding0 = 0;
            uint32_t padding1 = 0;
            uint32_t padding2 = 0;
        };

        static_assert(sizeof(SPHStickinessPushConstants) == 32);

        uint32_t nextPowerOfTwo(uint32_t value)
        {
            if (value <= 1)
            {
                return 1;
            }

            --value;

            value |= value >> 1;
            value |= value >> 2;
            value |= value >> 4;
            value |= value >> 8;
            value |= value >> 16;

            return value + 1;
        }

    }

    void SPHGpuSimulation::init(aiko::AssetSystem& assetSystem, const aiko::vector<SPHParticle>& particles, const aiko::vector<Shape>& shapes)
    {

        m_particleCount = static_cast<uint32_t>(particles.size());

        AIKO_ASSERT(m_particleCount <= MaxPackedParticleCount, "Packed GPU spring keys support at most 65536 particles");

        aiko::vector<aiko::vec4> positions;
        aiko::vector<aiko::vec4> previousPositions;
        aiko::vector<aiko::vec4> velocities;

        positions.reserve(m_particleCount);
        previousPositions.reserve(m_particleCount);
        velocities.reserve(m_particleCount);

        for (const SPHParticle& particle : particles)
        {
            positions.emplace_back(particle.position.x, particle.position.y, particle.position.z, 0.0f);
            previousPositions.emplace_back(particle.prevPosition.x, particle.prevPosition.y, particle.prevPosition.z, 0.0f);
            velocities.emplace_back(particle.velocity.x, particle.velocity.y, particle.velocity.z, 0.0f);
        }

        const aiko::ComputeBufferDesc positionBufferDesc
        {
            .format = aiko::ComputeBufferFormat::Vec4f,
            .count = MaxGpuParticles,
            .usage = aiko::ComputeBufferUsage::Storage | aiko::ComputeBufferUsage::TransferSrc | aiko::ComputeBufferUsage::TransferDst | aiko::ComputeBufferUsage::Vertex
        };

        const aiko::ComputeBufferDesc velocityBufferDesc
        {
            .format = aiko::ComputeBufferFormat::Vec4f,
            .count = MaxGpuParticles,
            .usage = aiko::ComputeBufferUsage::Storage | aiko::ComputeBufferUsage::TransferSrc | aiko::ComputeBufferUsage::TransferDst
        };

        const aiko::ComputeBufferDesc uintBufferDesc
        {
            .format = aiko::ComputeBufferFormat::Uint32,
            .count = MaxGpuParticles,
            .usage = aiko::ComputeBufferUsage::Storage | aiko::ComputeBufferUsage::TransferSrc
        };

        const aiko::ComputeBufferDesc springBufferDesc
        {
            .format = aiko::ComputeBufferFormat::Structured,
            .count = MaxSprings,
            .stride = sizeof(GpuSpring),
            .usage = aiko::ComputeBufferUsage::Storage | aiko::ComputeBufferUsage::TransferSrc | aiko::ComputeBufferUsage::TransferDst
        };

        m_positionBuffer.create(positionBufferDesc, nullptr);
        m_prevPositionBuffer.create(positionBufferDesc, nullptr);
        m_velocityBuffer.create(velocityBufferDesc, nullptr);
        m_velocityDeltaBuffer.create(velocityBufferDesc, nullptr);

        m_positionBuffer.update(0, m_particleCount, positions.data());
        m_prevPositionBuffer.update(0, m_particleCount, previousPositions.data());
        m_velocityBuffer.update(0, m_particleCount, velocities.data());

        m_densityBuffer.create(velocityBufferDesc, nullptr);
        m_pressureBuffer.create(velocityBufferDesc, nullptr);
        m_positionDeltaBuffer.create(velocityBufferDesc, nullptr);

        aiko::vector<GpuSpring> initialSprings(MaxSprings);
        m_springBuffer.create(springBufferDesc, initialSprings.data());
        m_previousSpringBuffer.create(springBufferDesc, initialSprings.data());

        m_cellKeyBuffer.create(uintBufferDesc, nullptr);
        m_particleIndexBuffer.create(uintBufferDesc, nullptr);

        // Shapes
        aiko::vector<GpuShape> gpuShapes;
        aiko::vector<GpuShapeEdge> gpuEdges;

        gpuShapes.reserve(shapes.size());

        for (const Shape& shape : shapes)
        {
            const uint32_t edgeStart = static_cast<uint32_t>(gpuEdges.size());

            for (const ShapeEdge& edge : shape.boundaryEdges())
            {
                const aiko::vec3 worldA = shape.asset().m_vertices[edge.a] + shape.position();

                const aiko::vec3 worldB = shape.asset().m_vertices[edge.b] + shape.position();

                gpuEdges.push_back(
                {
                    .a =
                    {
                        worldA.x,
                        worldA.y,
                        worldA.z,
                        0.0f
                    },

                    .b =
                    {
                        worldB.x,
                        worldB.y,
                        worldB.z,
                        0.0f
                    }
                });
            }

            gpuShapes.push_back(
            {
                .edgeStart = edgeStart,
                .edgeCount = static_cast<uint32_t>(shape.boundaryEdges().size())
            });
        }

        m_shapeCount = static_cast<uint32_t>(gpuShapes.size());
        m_shapeEdgeCount = static_cast<uint32_t>(gpuEdges.size());

        const aiko::ComputeBufferDesc shapeEdgeBufferDesc
        {
            .format = aiko::ComputeBufferFormat::Structured,
            .count = m_shapeEdgeCount,
            .stride = sizeof(GpuShapeEdge),
            .usage = aiko::ComputeBufferUsage::Storage | aiko::ComputeBufferUsage::TransferDst
        };

        const aiko::ComputeBufferDesc shapeBufferDesc
        {
            .format = aiko::ComputeBufferFormat::Structured,
            .count = m_shapeCount,
            .stride = sizeof(GpuShape),
            .usage = aiko::ComputeBufferUsage::Storage | aiko::ComputeBufferUsage::TransferDst
        };

        m_shapeEdgeBuffer.create(shapeEdgeBufferDesc, gpuEdges.data());
        m_shapeBuffer.create(shapeBufferDesc, gpuShapes.data());

        m_gravityShaderId = assetSystem.registerAndLoadAsset<aiko::ComputeShaderAsset>("sph/sph_gravity");
        m_predictShaderId = assetSystem.registerAndLoadAsset<aiko::ComputeShaderAsset>("sph/sph_predict");
        m_boundaryShaderId = assetSystem.registerAndLoadAsset<aiko::ComputeShaderAsset>("sph/sph_boundary");
        m_computeVelocityShaderId = assetSystem.registerAndLoadAsset<aiko::ComputeShaderAsset>("sph/sph_compute_velocity");
        m_hashShaderId = assetSystem.registerAndLoadAsset<aiko::ComputeShaderAsset>("sph/sph_hash");
        m_sortShaderId = assetSystem.registerAndLoadAsset<aiko::ComputeShaderAsset>("sph/sph_sort");
        m_cellRangeShaderId = assetSystem.registerAndLoadAsset<aiko::ComputeShaderAsset>("sph/sph_cell_ranges");
        m_clearCellsShaderId = assetSystem.registerAndLoadAsset<aiko::ComputeShaderAsset>("sph/sph_clear_cells");
        m_viscosityShaderId = assetSystem.registerAndLoadAsset<aiko::ComputeShaderAsset>("sph/sph_viscosity");
        m_applyViscosityShaderId = assetSystem.registerAndLoadAsset<aiko::ComputeShaderAsset>("sph/sph_apply_viscosity");
        m_densityShaderId = assetSystem.registerAndLoadAsset<aiko::ComputeShaderAsset>("sph/sph_density");
        m_relaxationShaderId = assetSystem.registerAndLoadAsset<aiko::ComputeShaderAsset>("sph/sph_relaxation");
        m_applyPositionDeltaShaderId = assetSystem.registerAndLoadAsset<aiko::ComputeShaderAsset>("sph/sph_apply_position_delta");

        m_clearSpringsShaderId = assetSystem.registerAndLoadAsset<aiko::ComputeShaderAsset>("sph/sph_clear_springs");
        m_migrateSpringsShaderId = assetSystem.registerAndLoadAsset<aiko::ComputeShaderAsset>("sph/sph_migrate_springs");
        m_generateSpringsShaderId = assetSystem.registerAndLoadAsset<aiko::ComputeShaderAsset>("sph/sph_generate_springs");
        m_springDisplacementShaderId = assetSystem.registerAndLoadAsset<aiko::ComputeShaderAsset>("sph/sph_spring_displacement");
        m_stickinessShaderId = assetSystem.registerAndLoadAsset<aiko::ComputeShaderAsset>("sph/sph_stickiness");

        m_shapeCollisionShaderId = assetSystem.registerAndLoadAsset<aiko::ComputeShaderAsset>("sph/sph_shape_collision");
    }

    void SPHGpuSimulation::update(aiko::RenderSystem& renderSystem, const SPHParameters& parameters, const WorldBounds& bounds)
    {
        if (m_particleCount == 0)
        {
            return;
        }

        const uint32_t sortParticleCount = nextPowerOfTwo(m_particleCount);
        AIKO_ASSERT(sortParticleCount <= MaxGpuParticles, "GPU SPH sort count exceeds particle capacity");

        // gravity
        const SPHGravityPushConstants gravityConstants
        {
            .dt = parameters.fixedDeltaTime,
            .gravity = parameters.gravity,
            .particleCount = m_particleCount,
            .padding = 0,

            .gravityDirection =
            {
                parameters.gravityDirection.x,
                parameters.gravityDirection.y,
                parameters.gravityDirection.z,
                0.0f
            }
        };

        aiko::ComputePass gravityPass{};
        gravityPass.name = "SPH Gravity";
        gravityPass.buffers.push_back({0, &m_velocityBuffer, aiko::ComputeAccess::ReadWrite});

        gravityPass.setPushConstants(gravityConstants);

        gravityPass.dispatch.groupsX = (m_particleCount + 63) / 64;

        renderSystem.dispatch(gravityPass, m_gravityShaderId);

        // Hash
        const aiko::vec3 halfSize = bounds.size * 0.5f;

        const float left = bounds.position.x - halfSize.x;
        const float bottom = bounds.position.y - halfSize.y;

        const uint32_t gridWidth = static_cast<uint32_t>(std::ceil(bounds.size.x / parameters.smoothingRadius));
        const uint32_t gridHeight = static_cast<uint32_t>(std::ceil(bounds.size.y / parameters.smoothingRadius));

        if (m_gridInitialized == false)
        {
            m_gridWidth = gridWidth;
            m_gridHeight = gridHeight;
            m_cellCount = m_gridWidth * m_gridHeight;

            const aiko::ComputeBufferDesc cellRangeDesc
            {
                .format = aiko::ComputeBufferFormat::Uint32,
                .count = m_cellCount,
                .usage = aiko::ComputeBufferUsage::Storage | aiko::ComputeBufferUsage::TransferSrc | aiko::ComputeBufferUsage::TransferDst
            };

            m_cellStartBuffer.create(cellRangeDesc, nullptr);
            m_cellEndBuffer.create(cellRangeDesc, nullptr);

            m_gridInitialized = true;
        }

        const SPHHashPushConstants hashConstants
        {
            .smoothingRadius = parameters.smoothingRadius,
            .particleCount = m_particleCount,
            .sortParticleCount = sortParticleCount,
            .gridWidth = gridWidth,

            .gridHeight = gridHeight,
            .padding0 = 0,
            .padding1 = 0,
            .padding2 = 0,

            .boundsMin =
            {
                left,
                bottom,
                0.0f,
                0.0f
            }
        };

        aiko::ComputePass hashPass{};
        hashPass.name = "SPH Hash";

        hashPass.buffers.push_back({0, &m_positionBuffer, aiko::ComputeAccess::Read});
        hashPass.buffers.push_back({ 1, &m_cellKeyBuffer, aiko::ComputeAccess::Write});
        hashPass.buffers.push_back({ 2, &m_particleIndexBuffer, aiko::ComputeAccess::Write});

        hashPass.setPushConstants(hashConstants);

        hashPass.dispatch.groupsX = (sortParticleCount + 63) / 64;
        hashPass.dispatch.groupsY = 1;
        hashPass.dispatch.groupsZ = 1;

        renderSystem.dispatch(hashPass, m_hashShaderId);

        // Sort
        for (uint32_t k = 2; k <= sortParticleCount; k <<= 1)
        {
            for (uint32_t j = k >> 1; j > 0; j >>= 1)
            {
                const SPHSortPushConstants sortConstants
                {
                    .particleCount = sortParticleCount,
                    .k = k,
                    .j = j,
                    .padding = 0
                };

                aiko::ComputePass sortPass{};
                sortPass.name = "SPH Sort";

                sortPass.buffers.push_back({0,&m_cellKeyBuffer,aiko::ComputeAccess::ReadWrite});
                sortPass.buffers.push_back({ 1, &m_particleIndexBuffer, aiko::ComputeAccess::ReadWrite});

                sortPass.setPushConstants(sortConstants);
                sortPass.dispatch.groupsX = (sortParticleCount + 63) / 64;

                renderSystem.dispatch( sortPass, m_sortShaderId);
            }
        }

        // Clear Cells
        const SPHClearCellsPushConstants clearConstants
        {
            .cellCount = m_cellCount
        };

        aiko::ComputePass clearPass{};
        clearPass.name = "SPH CellClear";

        clearPass.buffers.push_back({0,&m_cellStartBuffer,aiko::ComputeAccess::Write});
        clearPass.buffers.push_back({ 1, &m_cellEndBuffer, aiko::ComputeAccess::Write});

        clearPass.setPushConstants(clearConstants);

        clearPass.dispatch.groupsX = (m_cellCount + 63) / 64;

        renderSystem.dispatch(clearPass, m_clearCellsShaderId);

        // Range builder
        const SPHCellRangePushConstants rangeConstants
        {
            .particleCount = m_particleCount,
            .cellCount = m_cellCount
        };

        // Range builder
        aiko::ComputePass rangePass{};
        rangePass.name = "SPH CellRanges";

        rangePass.buffers.push_back({0, &m_cellKeyBuffer, aiko::ComputeAccess::Read});
        rangePass.buffers.push_back({1, &m_cellStartBuffer, aiko::ComputeAccess::Write});
        rangePass.buffers.push_back({2, &m_cellEndBuffer, aiko::ComputeAccess::Write});

        rangePass.setPushConstants(rangeConstants);

        rangePass.dispatch.groupsX = (m_particleCount + 63) / 64;

        renderSystem.dispatch(rangePass, m_cellRangeShaderId);

        // viscosity
        const SPHViscosityPushConstants viscosityConstants
        {
            .dt = parameters.fixedDeltaTime,
            .smoothingRadius = parameters.smoothingRadius,
            .sigma = parameters.sigma,
            .beta = parameters.beta,

            .particleCount = m_particleCount,
            .gridWidth = m_gridWidth,
            .gridHeight = m_gridHeight,
            .padding = 0,

            .boundsMin =
            {
                left,
                bottom,
                0.0f,
                0.0f
            }
        };

        aiko::ComputePass viscosityPass{};
        viscosityPass.name = "SPH Viscosity";

        viscosityPass.buffers.push_back({0, &m_positionBuffer, aiko::ComputeAccess::Read});
        viscosityPass.buffers.push_back({ 1, &m_velocityBuffer, aiko::ComputeAccess::Read});
        viscosityPass.buffers.push_back({2, &m_particleIndexBuffer, aiko::ComputeAccess::Read});
        viscosityPass.buffers.push_back({3, &m_cellStartBuffer, aiko::ComputeAccess::Read});
        viscosityPass.buffers.push_back({4, &m_cellEndBuffer, aiko::ComputeAccess::Read});
        viscosityPass.buffers.push_back({ 5, &m_velocityDeltaBuffer, aiko::ComputeAccess::Write});

        viscosityPass.setPushConstants(viscosityConstants);

        viscosityPass.dispatch.groupsX =(m_particleCount + 63) / 64;

        renderSystem.dispatch(viscosityPass, m_viscosityShaderId);

        // Apply viscosity
        const SPHApplyViscosityPushConstants applyConstants
        {
            .particleCount = m_particleCount
        };

        aiko::ComputePass applyPass{};
        applyPass.name = "SPH ApplyViscosity";

        applyPass.buffers.push_back({0, &m_velocityBuffer, aiko::ComputeAccess::ReadWrite});
        applyPass.buffers.push_back({1, &m_velocityDeltaBuffer, aiko::ComputeAccess::Read});

        applyPass.setPushConstants(applyConstants);

        applyPass.dispatch.groupsX = (m_particleCount + 63) / 64;

        renderSystem.dispatch(applyPass, m_applyViscosityShaderId);

        const SPHPredictPushConstants predictConstants
        {
            .dt = parameters.fixedDeltaTime,
            .particleCount = m_particleCount
        };

        // Prediction
        aiko::ComputePass predictPass{};
        predictPass.name = "SPH Predict";

        predictPass.buffers.push_back({ 0, &m_positionBuffer, aiko::ComputeAccess::ReadWrite});
        predictPass.buffers.push_back({ 1, &m_prevPositionBuffer, aiko::ComputeAccess::Write});
        predictPass.buffers.push_back({ 2, &m_velocityBuffer, aiko::ComputeAccess::Read});

        predictPass.setPushConstants(predictConstants);

        predictPass.dispatch.groupsX = (m_particleCount + 63) / 64;

        renderSystem.dispatch(predictPass, m_predictShaderId);

        aiko::ComputeBuffer* currentSpringBuffer = m_springBuffersFlipped ? &m_previousSpringBuffer : &m_springBuffer;
        aiko::ComputeBuffer* previousSpringBuffer = m_springBuffersFlipped ? &m_springBuffer : &m_previousSpringBuffer;

        // Clear current spring table
        const SPHClearSpringsPushConstants clearSpringsConstants
        {
            .maxSprings = MaxSprings
        };

        aiko::ComputePass clearSpringsPass{};
        clearSpringsPass.name = "SPH SpringClear";

        clearSpringsPass.buffers.push_back({ 0, currentSpringBuffer, aiko::ComputeAccess::Write });

        clearSpringsPass.setPushConstants(clearSpringsConstants);

        clearSpringsPass.dispatch.groupsX = (MaxSprings + 63) / 64;

        renderSystem.dispatch(clearSpringsPass, m_clearSpringsShaderId);

        // Migrate persistent springs
        const SPHMigrateSpringsPushConstants migrateSpringsConstants
        {
            .dt = parameters.fixedDeltaTime,
            .gamma = parameters.gamma,
            .plasticity = parameters.plasticity,
            .smoothingRadius = parameters.smoothingRadius,

            .maxSprings = MaxSprings,
            .padding0 = 0,
            .padding1 = 0,
            .padding2 = 0
        };

        aiko::ComputePass migrateSpringsPass{};
        migrateSpringsPass.name = "SPH SpringMigrate";

        migrateSpringsPass.buffers.push_back({0, &m_positionBuffer, aiko::ComputeAccess::Read});
        migrateSpringsPass.buffers.push_back({1, previousSpringBuffer, aiko::ComputeAccess::Read});
        migrateSpringsPass.buffers.push_back({2, currentSpringBuffer, aiko::ComputeAccess::ReadWrite});

        migrateSpringsPass.setPushConstants(migrateSpringsConstants);

        migrateSpringsPass.dispatch.groupsX = (MaxSprings + 63) / 64;

        renderSystem.dispatch(migrateSpringsPass, m_migrateSpringsShaderId);

        // Generate springs
        const SPHGenerateSpringsPushConstants springConstants
        {
            .smoothingRadius = parameters.smoothingRadius,
            .dt = parameters.fixedDeltaTime,
            .gamma = parameters.gamma,
            .plasticity = parameters.plasticity,

            .particleCount = m_particleCount,
            .gridWidth = m_gridWidth,
            .gridHeight = m_gridHeight,
            .maxSprings = MaxSprings,

            .boundsMin =
            {
                left,
                bottom,
                0.0f,
                0.0f
            }
        };

        aiko::ComputePass springPass{};
        springPass.name = "SPH SpringGenerate";

        springPass.buffers.push_back({ 0, &m_positionBuffer, aiko::ComputeAccess::Read });
        springPass.buffers.push_back({ 1, &m_particleIndexBuffer, aiko::ComputeAccess::Read });
        springPass.buffers.push_back({ 2, &m_cellStartBuffer, aiko::ComputeAccess::Read });
        springPass.buffers.push_back({ 3, &m_cellEndBuffer, aiko::ComputeAccess::Read });
        springPass.buffers.push_back({4, currentSpringBuffer, aiko::ComputeAccess::ReadWrite});

        springPass.setPushConstants(springConstants);

        springPass.dispatch.groupsX =(m_particleCount + 63) / 64;

        springPass.dispatch.groupsY = 1;
        springPass.dispatch.groupsZ = 1;

        renderSystem.dispatch(springPass, m_generateSpringsShaderId);

        // Spring displacement
        const SPHSpringDisplacementPushConstants springDisplacementConstants
        {
            .dt = parameters.fixedDeltaTime,
            .smoothingRadius = parameters.smoothingRadius,
            .springStiffness = parameters.springStiffness,
            .particleCount = m_particleCount,

            .maxSprings = MaxSprings,
            .gridWidth = m_gridWidth,
            .gridHeight = m_gridHeight,
            .padding = 0,

            .boundsMin =
            {
                left,
                bottom,
                0.0f,
                0.0f
            }
        };

        aiko::ComputePass springDisplacementPass{};
        springDisplacementPass.name = "SPH SpringDisplacement";

        springDisplacementPass.buffers.push_back({0, &m_positionBuffer, aiko::ComputeAccess::Read});
        springDisplacementPass.buffers.push_back({1, currentSpringBuffer, aiko::ComputeAccess::Read});
        springDisplacementPass.buffers.push_back({2, &m_particleIndexBuffer, aiko::ComputeAccess::Read});
        springDisplacementPass.buffers.push_back({3, &m_cellStartBuffer, aiko::ComputeAccess::Read});
        springDisplacementPass.buffers.push_back({4, &m_cellEndBuffer, aiko::ComputeAccess::Read});
        springDisplacementPass.buffers.push_back({5, &m_positionDeltaBuffer, aiko::ComputeAccess::Write});

        springDisplacementPass.setPushConstants(springDisplacementConstants);

        springDisplacementPass.dispatch.groupsX = (m_particleCount + 63) / 64;

        renderSystem.dispatch(springDisplacementPass, m_springDisplacementShaderId);

        // apply-position-delta
        const SPHApplyPositionDeltaPushConstants springDeltaConstants
        {
            .particleCount = m_particleCount
        };

        aiko::ComputePass applySpringDeltaPass{};
        applySpringDeltaPass.name = "SPH SpringApply";

        applySpringDeltaPass.buffers.push_back({0, &m_positionBuffer, aiko::ComputeAccess::ReadWrite});
        applySpringDeltaPass.buffers.push_back({1, &m_positionDeltaBuffer, aiko::ComputeAccess::Read});

        applySpringDeltaPass.setPushConstants(springDeltaConstants);

        applySpringDeltaPass.dispatch.groupsX = (m_particleCount + 63) / 64;

        renderSystem.dispatch(applySpringDeltaPass, m_applyPositionDeltaShaderId);

        m_springBuffersFlipped = !m_springBuffersFlipped;

        // density
        const SPHDensityPushConstants densityConstants
        {
            .smoothingRadius = parameters.smoothingRadius,
            .restDensity = parameters.restDensity,
            .pressureStiffness = parameters.pressureStiffness,
            .nearPressureStiffness = parameters.nearPressureStiffness,
            .particleCount = m_particleCount,
            .gridWidth = m_gridWidth,
            .gridHeight = m_gridHeight,
            .padding = 0,
            .boundsMin ={ left, bottom, 0.0f, 0.0f}
        };

        aiko::ComputePass densityPass{};
        densityPass.name = "SPH Density";

        densityPass.buffers.push_back({0, &m_positionBuffer, aiko::ComputeAccess::Read});
        densityPass.buffers.push_back({1, &m_particleIndexBuffer, aiko::ComputeAccess::Read});
        densityPass.buffers.push_back({2, &m_cellStartBuffer, aiko::ComputeAccess::Read});
        densityPass.buffers.push_back({3, &m_cellEndBuffer, aiko::ComputeAccess::Read});
        densityPass.buffers.push_back({4, &m_densityBuffer, aiko::ComputeAccess::Write});
        densityPass.buffers.push_back({5, &m_pressureBuffer, aiko::ComputeAccess::Write});

        densityPass.setPushConstants(densityConstants);

        densityPass.dispatch.groupsX = (m_particleCount + 63) / 64;

        renderSystem.dispatch(densityPass, m_densityShaderId);

        // relaxation
        const SPHRelaxationPushConstants relaxationConstants
        {
            .dt = parameters.fixedDeltaTime,
            .smoothingRadius = parameters.smoothingRadius,
            .particleCount = m_particleCount,
            .gridWidth = m_gridWidth,
            .gridHeight = m_gridHeight,
            .boundsMin ={ left, bottom, 0.0f, 0.0f}
        };

        aiko::ComputePass relaxationPass{};
        relaxationPass.name = "SPH Relaxation";

        relaxationPass.buffers.push_back({0, &m_positionBuffer, aiko::ComputeAccess::Read});
        relaxationPass.buffers.push_back({1, &m_particleIndexBuffer, aiko::ComputeAccess::Read});
        relaxationPass.buffers.push_back({2, &m_cellStartBuffer, aiko::ComputeAccess::Read});
        relaxationPass.buffers.push_back({3, &m_cellEndBuffer, aiko::ComputeAccess::Read});
        relaxationPass.buffers.push_back({4, &m_pressureBuffer, aiko::ComputeAccess::Read});
        relaxationPass.buffers.push_back({5, &m_positionDeltaBuffer, aiko::ComputeAccess::Write});

        relaxationPass.setPushConstants(relaxationConstants);

        relaxationPass.dispatch.groupsX = (m_particleCount + 63) / 64;

        renderSystem.dispatch(relaxationPass, m_relaxationShaderId);

        // apply
        const SPHApplyPositionDeltaPushConstants deltaConstants
        {
            .particleCount = m_particleCount
        };

        aiko::ComputePass deltaPass{};
        deltaPass.name = "SPH RelaxationApply";

        deltaPass.buffers.push_back({0, &m_positionBuffer, aiko::ComputeAccess::ReadWrite});
        deltaPass.buffers.push_back({1, &m_positionDeltaBuffer, aiko::ComputeAccess::Read});

        deltaPass.setPushConstants(deltaConstants);

        deltaPass.dispatch.groupsX = (m_particleCount + 63) / 64;

        renderSystem.dispatch(deltaPass, m_applyPositionDeltaShaderId);

        // Stickiness
        const SPHStickinessPushConstants stickinessConstants
        {
            .dt = parameters.fixedDeltaTime,
            .maxStickiness = parameters.maxStickiness,
            .kStick = parameters.kStick,
            .particleCount = m_particleCount,

            .shapeCount = m_shapeCount,
            .padding0 = 0,
            .padding1 = 0,
            .padding2 = 0
        };

        aiko::ComputePass stickinessPass{};
        stickinessPass.name = "SPH Stickiness";

        stickinessPass.buffers.push_back({ 0, &m_positionBuffer, aiko::ComputeAccess::ReadWrite });
        stickinessPass.buffers.push_back({ 1, &m_shapeEdgeBuffer, aiko::ComputeAccess::Read });
        stickinessPass.buffers.push_back({ 2, &m_shapeBuffer, aiko::ComputeAccess::Read });

        stickinessPass.setPushConstants(stickinessConstants);

        stickinessPass.dispatch.groupsX = (m_particleCount + 63) / 64;

        renderSystem.dispatch(stickinessPass, m_stickinessShaderId);

        // Shape collision
        const SPHShapeCollisionPushConstants shapeCollisionConstants
        {
            .particleRadius = parameters.particleRadius,
            .particleCount = m_particleCount,
            .shapeCount = m_shapeCount,
            .padding = 0
        };

        aiko::ComputePass shapeCollisionPass{};
        shapeCollisionPass.name = "SPH ShapeCollision";

        shapeCollisionPass.buffers.push_back({ 0, &m_positionBuffer, aiko::ComputeAccess::ReadWrite });
        shapeCollisionPass.buffers.push_back({ 1, &m_shapeEdgeBuffer, aiko::ComputeAccess::Read });
        shapeCollisionPass.buffers.push_back({ 2, &m_shapeBuffer, aiko::ComputeAccess::Read});

        shapeCollisionPass.setPushConstants(shapeCollisionConstants);

        shapeCollisionPass.dispatch.groupsX = (m_particleCount + 63) / 64;

        renderSystem.dispatch(shapeCollisionPass, m_shapeCollisionShaderId);

        // boundary
        const SPHBoundaryPushConstants boundaryConstants
        {
            .particleRadius = parameters.particleRadius,
            .particleCount = m_particleCount,
            .boundsPosition = { bounds.position.x, bounds.position.y, bounds.position.z, 0.0f},
            .boundsSize = { bounds.size.x, bounds.size.y, bounds.size.z, 0.0f }
        };

        aiko::ComputePass boundaryPass{};
        boundaryPass.name = "SPH Boundary";

        boundaryPass.buffers.push_back({0, &m_positionBuffer, aiko::ComputeAccess::ReadWrite});
        boundaryPass.buffers.push_back({1, &m_prevPositionBuffer, aiko::ComputeAccess::ReadWrite});

        boundaryPass.setPushConstants(boundaryConstants);

        boundaryPass.dispatch.groupsX = (m_particleCount + 63) / 64;

        renderSystem.dispatch(boundaryPass, m_boundaryShaderId);

        // velocity
        const SPHComputeVelocityPushConstants velocityConstants
        {
            .dt = parameters.fixedDeltaTime,
            .particleCount = m_particleCount
        };

        aiko::ComputePass velocityPass{};
        velocityPass.name = "SPH Velocity";

        velocityPass.buffers.push_back({0, &m_positionBuffer, aiko::ComputeAccess::Read});
        velocityPass.buffers.push_back({1, &m_prevPositionBuffer, aiko::ComputeAccess::Read});
        velocityPass.buffers.push_back({2, &m_velocityBuffer, aiko::ComputeAccess::Write});

        velocityPass.setPushConstants(velocityConstants);

        velocityPass.dispatch.groupsX = (m_particleCount + 63) / 64;

        renderSystem.dispatch(velocityPass, m_computeVelocityShaderId);

    }

    void SPHGpuSimulation::spawnParticles(const aiko::vector<SPHParticle>& particles)
    {
        if (particles.empty())
        {
            return;
        }

        const uint32_t spawnCount = static_cast<uint32_t>(particles.size());

        AIKO_ASSERT(m_particleCount + spawnCount <= MaxGpuParticles, "GPU SPH particle capacity exceeded");
        AIKO_ASSERT(m_particleCount + spawnCount <= MaxPackedParticleCount, "Packed GPU spring keys support at most 65536 particles");

        aiko::vector<aiko::vec4> positions;
        aiko::vector<aiko::vec4> previousPositions;
        aiko::vector<aiko::vec4> velocities;

        positions.reserve(spawnCount);
        previousPositions.reserve(spawnCount);
        velocities.reserve(spawnCount);

        for (const SPHParticle& particle : particles)
        {
            positions.emplace_back(particle.position.x, particle.position.y, particle.position.z, 0.0f);
            previousPositions.emplace_back(particle.prevPosition.x, particle.prevPosition.y, particle.prevPosition.z, 0.0f);
            velocities.emplace_back(particle.velocity.x, particle.velocity.y, particle.velocity.z, 0.0f);
        }

        m_positionBuffer.update(m_particleCount, spawnCount, positions.data());
        m_prevPositionBuffer.update(m_particleCount, spawnCount, previousPositions.data());
        m_velocityBuffer.update(m_particleCount, spawnCount, velocities.data());

        m_particleCount += spawnCount;
    }

    void SPHGpuSimulation::updateShapes(const aiko::vector<Shape>& shapes)
    {
        AIKO_ASSERT(shapes.size() == m_shapeCount, "GPU SPH shape count changed after initialization");

        aiko::vector<GpuShapeEdge> gpuEdges;
        gpuEdges.reserve(m_shapeEdgeCount);

        for (const Shape& shape : shapes)
        {
            for (const ShapeEdge& edge : shape.boundaryEdges())
            {
                const aiko::vec3 worldA = shape.asset().m_vertices[edge.a] + shape.position();

                const aiko::vec3 worldB = shape.asset().m_vertices[edge.b] + shape.position();

                gpuEdges.push_back(
                {
                    .a =
                    {
                        worldA.x,
                        worldA.y,
                        worldA.z,
                        0.0f
                    },

                    .b =
                    {
                        worldB.x,
                        worldB.y,
                        worldB.z,
                        0.0f
                    }
                });
            }
        }

        AIKO_ASSERT(gpuEdges.size() == m_shapeEdgeCount, "GPU SPH shape topology changed after initialization");

        m_shapeEdgeBuffer.update(0, m_shapeEdgeCount, gpuEdges.data());
    }

}
