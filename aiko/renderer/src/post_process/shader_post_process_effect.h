
#pragma once

#include <string>
#include <utility>

#include "models/material.h"
#include "post_process_effect.h"

namespace aiko
{
    class ShaderPostProcessEffect final : public PostProcessEffect
    {
    public:
        ShaderPostProcessEffect(std::string name, AssetId shaderId, PostProcessDomain inputDomain, PostProcessDomain outputDomain);

        PostProcessDomain inputDomain() const override;
        PostProcessDomain outputDomain() const override;

        void execute(PostProcessContext& context) override;

        Material& material();
        const Material& material() const;

    private:
        PostProcessDomain m_inputDomain;
        PostProcessDomain m_outputDomain;
        Material m_material;
    };
}
