#pragma once

#include <layers/layer.h>
#include <models/material.h>

#include <array>

namespace shadertoy
{

    class VulkanShaderToy final : public aiko::Layer
    {
    protected:
        void init() override;
        void update() override;
        void render() override;

    private:

        void setChannel(uint32_t index, aiko::AssetId textureId, const aiko::ivec2& resolution, const aiko::SamplerState& sampler = {});

        aiko::Material m_material;

        float m_time = 0.0f;
        int m_frame = 0;

        aiko::vec2 m_mousePosition = {};
        aiko::vec2 m_mouseClickPosition = {};

        bool m_hasMouseClick = false;

        std::array<float, 4> m_channelTimes{};
        std::array<aiko::vec3, 4> m_channelResolutions{};

        // Controllers
        size_t m_currentShader = 0;
        void nextShader();
        void previousShader();
        void randomShader();
        void loadCurrentShader();

    };

}
