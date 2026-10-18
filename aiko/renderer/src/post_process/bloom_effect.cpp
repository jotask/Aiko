#include "bloom_effect.h"

#include <algorithm>
#include <cmath>

#include "post_process_context.h"

namespace aiko
{
    namespace
    {
        void configureFullscreenMaterial(Material& material, AssetId shaderId)
        {
            AIKO_ASSERT(shaderId != InvalidAssetId, "Bloom shader ID is invalid");

            material.m_shaderId = shaderId;
            material.m_lit = false;
            material.m_useVertexColor = false;

            material.m_renderState.depthTest = false;
            material.m_renderState.depthWrite = false;
            material.m_renderState.blend = false;
            material.m_renderState.cullMode = CullMode::None;
        }

        u32 halfResolution(u32 size)
        {
            return size / 2u + size % 2u;
        }
    }

    BloomEffect::BloomEffect(AssetId extractShader, AssetId downsampleShader, AssetId upsampleShader, AssetId compositeShader)
        : PostProcessEffect("Bloom", true)
    {
        configureFullscreenMaterial(m_extractMaterial, extractShader);
        configureFullscreenMaterial(m_downsampleMaterial, downsampleShader);
        configureFullscreenMaterial(m_upsampleMaterial, upsampleShader);
        configureFullscreenMaterial(m_compositeMaterial, compositeShader);

        m_downsampleMaterial.setFloat("u_filterScale", 1.0f);
        m_upsampleMaterial.setFloat("u_filterScale", 1.0f);
    }

    PostProcessDomain BloomEffect::inputDomain() const
    {
        return PostProcessDomain::HDR;
    }

    PostProcessDomain BloomEffect::outputDomain() const
    {
        return PostProcessDomain::HDR;
    }

    void BloomEffect::execute(PostProcessContext& context)
    {
        const ivec2 size = context.outputTarget().size();

        AIKO_ASSERT(size.x > 0 && size.y > 0, "Invalid Bloom dimensions");

        const u32 fullWidth = static_cast<u32>(size.x);
        const u32 fullHeight = static_cast<u32>(size.y);

        const u32 width0 = halfResolution(fullWidth);
        const u32 height0 = halfResolution(fullHeight);

        const u32 width1 = halfResolution(width0);
        const u32 height1 = halfResolution(height0);

        const u32 width2 = halfResolution(width1);
        const u32 height2 = halfResolution(height1);

        // Downsample pyramid: half, quarter, eighth resolution.
        RenderTarget& level0 = context.scratch(0, width0, height0, PostProcessDomain::HDR);
        RenderTarget& level1 = context.scratch(1, width1, height1, PostProcessDomain::HDR);
        RenderTarget& level2 = context.scratch(2, width2, height2, PostProcessDomain::HDR);

        // Upsample results use separate targets to avoid read/write aliasing.
        RenderTarget& up1 = context.scratch(3, width1, height1, PostProcessDomain::HDR);
        RenderTarget& up0 = context.scratch(4, width0, height0, PostProcessDomain::HDR);

        // Pass 1: threshold the original HDR scene.
        m_extractMaterial.setFloat("u_threshold", m_threshold);

        context.renderFullscreen(m_extractMaterial, context.inputTexture(), level0);

        SamplerState sampler{};
        sampler.wrapU = TextureWrapMode::Clamp;
        sampler.wrapV = TextureWrapMode::Clamp;

        // Pass 2: downsample half -> quarter.
        context.renderFullscreen(m_downsampleMaterial, level0.colorTexture(), level1);

        // Pass 3: downsample quarter -> eighth.
        context.renderFullscreen(m_downsampleMaterial, level1.colorTexture(), level2);

        // Pass 4: combine eighth-resolution glow with quarter-resolution glow.
        m_upsampleMaterial.setTexture("u_highTexture", &level1.colorTexture(), sampler);

        context.renderFullscreen(m_upsampleMaterial, level2.colorTexture(), up1);

        // Pass 5: combine accumulated glow with half-resolution glow.
        m_upsampleMaterial.setTexture("u_highTexture", &level0.colorTexture(), sampler);

        context.renderFullscreen(m_upsampleMaterial, up1.colorTexture(), up0);

        // Pass 6: HDR composition.
        m_compositeMaterial.setFloat("u_intensity", m_intensity);
        m_compositeMaterial.setTexture("u_bloomTexture", &up0.colorTexture(), sampler);

        context.renderFullscreen(m_compositeMaterial, context.inputTexture(), context.outputTarget());
    }

    void BloomEffect::setThreshold(float threshold)
    {
        AIKO_ASSERT(std::isfinite(threshold) && threshold >= 0.0f, "Bloom threshold must be finite and non-negative");
        m_threshold = threshold;
    }

    void BloomEffect::setIntensity(float intensity)
    {
        AIKO_ASSERT(std::isfinite(intensity) && intensity >= 0.0f, "Bloom intensity must be finite and non-negative");
        m_intensity = intensity;
    }
}
