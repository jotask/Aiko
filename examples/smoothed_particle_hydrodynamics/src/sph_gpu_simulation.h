#pragma once

#include "sph_types.h"

#include <models/compute_buffer.h>
#include <types/compute_pass.h>

namespace aiko
{
    class AssetSystem;
    class RenderSystem;
}

namespace sph
{

    class SPHGpuSimulation
    {
    public:
        void init(aiko::AssetSystem& assetSystem, const aiko::vector<SPHParticle>& particles);

        void update(aiko::RenderSystem& renderSystem, const SPHParameters& parameters, const WorldBounds& bounds);

        const aiko::ComputeBuffer& positionBuffer() const { return m_positionBuffer; }
        uint32_t particleCount() const { return m_particleCount; }

    private:
        aiko::ComputeBuffer m_positionBuffer;
        aiko::ComputeBuffer m_velocityBuffer;

        aiko::ComputeBuffer m_cellKeyBuffer;
        aiko::ComputeBuffer m_particleIndexBuffer;

        aiko::ComputeBuffer m_cellStartBuffer;
        aiko::ComputeBuffer m_cellEndBuffer;

        aiko::AssetId m_updateShaderId = aiko::InvalidAssetId;
        aiko::AssetId m_hashShaderId = aiko::InvalidAssetId;
        aiko::AssetId m_sortShaderId = aiko::InvalidAssetId;
        aiko::AssetId m_cellRangeShaderId = aiko::InvalidAssetId;
        aiko::AssetId m_clearCellsShaderId = aiko::InvalidAssetId;

        uint32_t m_particleCount = 0;

        bool m_gridInitialized = false;
        uint32_t m_gridWidth = 0;
        uint32_t m_gridHeight = 0;
        uint32_t m_cellCount = 0;
    };

}