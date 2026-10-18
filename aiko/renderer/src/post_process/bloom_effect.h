#pragma once

#include "post_process_effect.h"
#include "models/material.h"

namespace aiko
{
    class BloomEffect final : public PostProcessEffect
    {
    public:
        BloomEffect(AssetId extractShader, AssetId downsampleShader, AssetId upsampleShader, AssetId compositeShader);

        PostProcessDomain inputDomain() const override;
        PostProcessDomain outputDomain() const override;

        void execute(PostProcessContext& context) override;

        void setThreshold(float threshold);
        void setIntensity(float intensity);

        float threshold() const { return m_threshold; }
        float intensity() const { return m_intensity; }

    private:
        Material m_extractMaterial;
        Material m_downsampleMaterial;
        Material m_upsampleMaterial;
        Material m_compositeMaterial;

        float m_threshold = 1.0f;
        float m_intensity = 0.5f;
    };
}
