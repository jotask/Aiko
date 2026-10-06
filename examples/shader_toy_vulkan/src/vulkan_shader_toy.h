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
        int m_frame = 0;

        aiko::vec2 m_mousePosition = {};
        aiko::vec2 m_mouseClickPosition = {};

        bool m_hasMouseClick = false;
    };

}