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
        aiko::ComputeBuffer m_prevPositionBuffer;
        aiko::ComputeBuffer m_velocityBuffer;
        aiko::ComputeBuffer m_velocityDeltaBuffer;

        aiko::ComputeBuffer m_densityBuffer;
        aiko::ComputeBuffer m_pressureBuffer;
        aiko::ComputeBuffer m_positionDeltaBuffer;

        aiko::ComputeBuffer m_springBuffer;

        aiko::ComputeBuffer m_cellKeyBuffer;
        aiko::ComputeBuffer m_particleIndexBuffer;

        aiko::ComputeBuffer m_cellStartBuffer;
        aiko::ComputeBuffer m_cellEndBuffer;

        aiko::AssetId m_gravityShaderId = aiko::InvalidAssetId;
        aiko::AssetId m_predictShaderId = aiko::InvalidAssetId;
        aiko::AssetId m_boundaryShaderId = aiko::InvalidAssetId;
        aiko::AssetId m_computeVelocityShaderId = aiko::InvalidAssetId;
        aiko::AssetId m_hashShaderId = aiko::InvalidAssetId;
        aiko::AssetId m_sortShaderId = aiko::InvalidAssetId;
        aiko::AssetId m_cellRangeShaderId = aiko::InvalidAssetId;
        aiko::AssetId m_clearCellsShaderId = aiko::InvalidAssetId;
        aiko::AssetId m_viscosityShaderId = aiko::InvalidAssetId;
        aiko::AssetId m_applyViscosityShaderId = aiko::InvalidAssetId;
        aiko::AssetId m_densityShaderId = aiko::InvalidAssetId;
        aiko::AssetId m_relaxationShaderId = aiko::InvalidAssetId;
        aiko::AssetId m_applyPositionDeltaShaderId = aiko::InvalidAssetId;

        aiko::AssetId m_generateSpringsShaderId = aiko::InvalidAssetId;
        aiko::AssetId m_springPlasticityShaderId = aiko::InvalidAssetId;
        aiko::AssetId m_springDisplacementShaderId = aiko::InvalidAssetId;

        uint32_t m_particleCount = 0;

        bool m_gridInitialized = false;
        uint32_t m_gridWidth = 0;
        uint32_t m_gridHeight = 0;
        uint32_t m_cellCount = 0;
    };

}