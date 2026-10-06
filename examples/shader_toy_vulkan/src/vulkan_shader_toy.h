#pragma once

#include "layers/layer.h"

#include "models/material.h"

namespace shadertoy
{

    class VulkanShaderToy final : public aiko::Layer
    {
    protected:
        void init() override;
        void update() override;
        void render() override;

    private:
        aiko::Material m_material;

        float m_time = 0.0f;
    };

}