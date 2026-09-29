#include "sph_gpu_simulation.h"

#include "assets/types/shader_asset.h"
#include "systems/asset_system.h"
#include "systems/render_system.h"

#include <cmath>

namespace sph
{

    namespace
    {

        struct SPHIntegratePushConstants
        {
            float dt = 0.0f;
            float gravity = 0.0f;
            float particleRadius = 0.0f;
            uint32_t particleCount = 0;

            alignas(16) aiko::vec4 gravityDirection = {};
            alignas(16) aiko::vec4 boundsPosition = {};
            alignas(16) aiko::vec4 boundsSize = {};
        };

        static_assert(sizeof(SPHIntegratePushConstants) == 64);

        struct SPHHashPushConstants
        {
            float smoothingRadius = 0.0f;
            uint32_t particleCount = 0;
            uint32_t gridWidth = 0;
            uint32_t gridHeight = 0;

            alignas(16) aiko::vec4 boundsMin = {};
        };

        static_assert(sizeof(SPHHashPushConstants) == 32);

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

    }

    void SPHGpuSimulation::init(aiko::AssetSystem& assetSystem, const aiko::vector<SPHParticle>& particles)
    {
        m_particleCount = static_cast<uint32_t>(particles.size());

        aiko::vector<aiko::vec4> positions;
        aiko::vector<aiko::vec4> velocities;

        positions.reserve(m_particleCount);
        velocities.reserve(m_particleCount);

        for (const SPHParticle& particle : particles)
        {
            positions.emplace_back(particle.position.x, particle.position.y, particle.position.z, 0.0f);
            velocities.emplace_back(particle.velocity.x, particle.velocity.y, particle.velocity.z, 0.0f);
        }

        const aiko::ComputeBufferDesc positionBufferDesc
        {
            .format = aiko::ComputeBufferFormat::Vec4f,
            .count = m_particleCount,
            .usage = aiko::ComputeBufferUsage::Storage | aiko::ComputeBufferUsage::TransferSrc | aiko::ComputeBufferUsage::TransferDst | aiko::ComputeBufferUsage::Vertex
        };

        const aiko::ComputeBufferDesc velocityBufferDesc
        {
            .format = aiko::ComputeBufferFormat::Vec4f,
            .count = m_particleCount,
            .usage = aiko::ComputeBufferUsage::Storage | aiko::ComputeBufferUsage::TransferSrc | aiko::ComputeBufferUsage::TransferDst
        };

        const aiko::ComputeBufferDesc uintBufferDesc
        {
            .format = aiko::ComputeBufferFormat::Uint32,
            .count = m_particleCount,
            .usage = aiko::ComputeBufferUsage::Storage | aiko::ComputeBufferUsage::TransferSrc
        };

        m_cellKeyBuffer.create(uintBufferDesc, nullptr);
        m_particleIndexBuffer.create(uintBufferDesc, nullptr);

        m_positionBuffer.create(positionBufferDesc, positions.data());
        m_velocityBuffer.create(velocityBufferDesc, velocities.data());

        m_updateShaderId = assetSystem.registerAndLoadAsset<aiko::ComputeShaderAsset>("sph_integrate");
        m_hashShaderId = assetSystem.registerAndLoadAsset<aiko::ComputeShaderAsset>("sph_hash");
        m_sortShaderId = assetSystem.registerAndLoadAsset<aiko::ComputeShaderAsset>("sph_sort");
        m_cellRangeShaderId = assetSystem.registerAndLoadAsset<aiko::ComputeShaderAsset>("sph_cell_ranges");
        m_clearCellsShaderId = assetSystem.registerAndLoadAsset<aiko::ComputeShaderAsset>("sph_clear_cells");
    }

    void SPHGpuSimulation::update(aiko::RenderSystem& renderSystem, const SPHParameters& parameters, const WorldBounds& bounds)
    {
        if (m_particleCount == 0)
        {
            return;
        }

        // Settings
        const SPHIntegratePushConstants constants
        {
            .dt = parameters.fixedDeltaTime,
            .gravity = parameters.gravity,
            .particleRadius = parameters.particleRadius,
            .particleCount = m_particleCount,

            .gravityDirection = { parameters.gravityDirection.x, parameters.gravityDirection.y, parameters.gravityDirection.z, 0.0f},
            .boundsPosition = { bounds.position.x, bounds.position.y, bounds.position.z, 0.0f },
            .boundsSize = { bounds.size.x, bounds.size.y, bounds.size.z, 0.0f }
        };

        aiko::ComputePass pass{};

        pass.buffers.push_back({ 0, &m_positionBuffer, aiko::ComputeAccess::ReadWrite});
        pass.buffers.push_back({ 1, &m_velocityBuffer, aiko::ComputeAccess::ReadWrite});

        pass.setPushConstants(constants);

        pass.dispatch.groupsX = (m_particleCount + 63) / 64;
        pass.dispatch.groupsY = 1;
        pass.dispatch.groupsZ = 1;

        renderSystem.dispatch(pass, m_updateShaderId);

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
            .gridWidth = gridWidth,
            .gridHeight = gridHeight,
            .boundsMin =
            {
                left,
                bottom,
                0.0f,
                0.0f
            }
        };

        aiko::ComputePass hashPass{};

        hashPass.buffers.push_back({0, &m_positionBuffer, aiko::ComputeAccess::Read});
        hashPass.buffers.push_back({ 1, &m_cellKeyBuffer, aiko::ComputeAccess::Write});
        hashPass.buffers.push_back({ 2, &m_particleIndexBuffer, aiko::ComputeAccess::Write});

        hashPass.setPushConstants(hashConstants);

        hashPass.dispatch.groupsX = (m_particleCount + 63) / 64;
        hashPass.dispatch.groupsY = 1;
        hashPass.dispatch.groupsZ = 1;

        renderSystem.dispatch(hashPass, m_hashShaderId);

        // Sort
        for (uint32_t k = 2; k <= m_particleCount; k <<= 1)
        {
            for (uint32_t j = k >> 1; j > 0; j >>= 1)
            {
                const SPHSortPushConstants sortConstants
                {
                    .particleCount = m_particleCount,
                    .k = k,
                    .j = j,
                    .padding = 0
                };

                aiko::ComputePass sortPass{};

                sortPass.buffers.push_back({0,&m_cellKeyBuffer,aiko::ComputeAccess::ReadWrite});
                sortPass.buffers.push_back({ 1, &m_particleIndexBuffer, aiko::ComputeAccess::ReadWrite});

                sortPass.setPushConstants(sortConstants);
                sortPass.dispatch.groupsX = (m_particleCount + 63) / 64;

                renderSystem.dispatch( sortPass, m_sortShaderId);
            }
        }

        // Clear Cells
        const SPHClearCellsPushConstants clearConstants
        {
            .cellCount = m_cellCount
        };

        aiko::ComputePass clearPass{};

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

        aiko::ComputePass rangePass{};

        rangePass.buffers.push_back({0, &m_cellKeyBuffer, aiko::ComputeAccess::Read});
        rangePass.buffers.push_back({1, &m_cellStartBuffer, aiko::ComputeAccess::Write});
        rangePass.buffers.push_back({2, &m_cellEndBuffer, aiko::ComputeAccess::Write});

        rangePass.setPushConstants(rangeConstants);

        rangePass.dispatch.groupsX = (m_particleCount + 63) / 64;

        renderSystem.dispatch(rangePass, m_cellRangeShaderId);

    }

}