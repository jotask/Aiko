
#pragma once

#include "post_process_effect.h"
#include "models/material.h"

namespace aiko
{
    class ToneMappingEffect final : public PostProcessEffect
    {
    public:
        explicit ToneMappingEffect(AssetId shaderId);

        PostProcessDomain inputDomain() const override;
        PostProcessDomain outputDomain() const override;

        void execute(PostProcessContext& context) override;

        void setExposure(float exposure);
        float exposure() const;

    private:
        Material m_material;
        float m_exposure = 1.0f;
    };
}
