#include "shadow_renderer.h"

#include "renderer/Irenderdevice.h"

#include <assets/iasset_provider.h>
#include <math/math.h>

#include <cmath>
#include <algorithm>

namespace aiko::renderer
{
    void ShadowRenderer::init(IAssetRegistry& registry, u32 resolution)
    {
        AIKO_ASSERT(resolution > 0, "Shadow resolution must be positive");

        dispose();

        m_depthTarget.create({
            .width = static_cast<int>(resolution),
            .height = static_cast<int>(resolution),
            .colorFormat = TextureFormat::None,
        });

        AIKO_ASSERT(m_depthTarget.isValid(), "Failed to initialize shadow depth target");

        m_resolution = resolution;

        const AssetId depthShaderId = registry.registerShader("renderer://shadow_depth.vs", "renderer://shadow_depth.fs");
        AIKO_ASSERT(depthShaderId != InvalidAssetId, "Failed to register shadow depth shader");

        m_depthMaterial = Material{};
        m_depthMaterial.m_shaderId = depthShaderId;
        m_depthMaterial.m_renderState.depthTest = true;
        m_depthMaterial.m_renderState.depthWrite = true;
        m_depthMaterial.m_renderState.blend = false;
    }

    void ShadowRenderer::dispose()
    {
        m_depthMaterial = Material{};
        m_shadowData = {};
        m_depthTarget.unload();
        m_resolution = 0;
    }

    void ShadowRenderer::render(IRenderDevice& device, const FrameData& sceneFrame, const PreparedScenePass& scene)
    {
        AIKO_ASSERT(isValid(), "ShadowRenderer is not initialized");

        m_shadowData = {};

        const LightData* shadowLight = nullptr;
        u32 shadowLightIndex = 0;

        // Only lights uploaded to the GPU are eligible.
        const u32 lightCount = static_cast<u32>(std::min<size_t>(sceneFrame.lights.size(), MaxFrameLights));

        for (u32 i = 0; i < lightCount; ++i)
        {
            const LightData& light = sceneFrame.lights[i];

            if (light.intensity <= 0.0f)
            {
                continue;
            }

            if (light.type == LightType::Directional || light.type == LightType::Spot)
            {
                shadowLight = &light;
                shadowLightIndex = i;
                break;
            }
        }

        if (shadowLight == nullptr)
        {
            return;
        }

        AIKO_ASSERT(math::isfinite(shadowLight->direction) && math::length(shadowLight->direction) > 0.0001f, "Shadow light direction must be finite and nonzero");

        const vec3 direction = math::normalize(shadowLight->direction);

        const vec3 up = std::abs(direction.y) > 0.99f
            ? vec3(0.0f, 0.0f, 1.0f)
            : vec3(0.0f, 1.0f, 0.0f);

        FrameData lightFrame{};

        if (shadowLight->type == LightType::Directional)
        {
            constexpr float shadowDistance = 40.0f;
            constexpr float shadowExtent = 30.0f;

            const vec3 center = sceneFrame.cameraPosition;
            const vec3 eye = center - direction * shadowDistance;

            lightFrame.view = math::lookAt(eye, center, up);
            lightFrame.projection = math::ortho(-shadowExtent, shadowExtent, -shadowExtent, shadowExtent, 0.1f, shadowDistance * 2.0f);
        }
        else
        {
            // Spotlight: a perspective camera positioned at the light.
            AIKO_ASSERT(math::isfinite(shadowLight->position), "Spotlight position must be finite");
            AIKO_ASSERT(shadowLight->range > 0.1f, "Spotlight range must exceed the shadow near plane");
            AIKO_ASSERT(shadowLight->outerCos > 0.0f && shadowLight->outerCos < 1.0f, "Invalid spotlight outer cone");

            const vec3 position = shadowLight->position;

            lightFrame.view = math::lookAt(position, position + direction, up);

            const float fov = 2.0f * math::degrees(std::acos(shadowLight->outerCos));

            lightFrame.projection = math::perspective(fov, 1.0f, 0.1f, shadowLight->range);
        }

        const PassDescription shadowPass{
            .width = m_resolution,
            .height = m_resolution,
            .colorLoadOp = AttachmentLoadOp::DontCare,
            .depthLoadOp = AttachmentLoadOp::Clear,
        };

        device.prepareMaterial(m_depthMaterial);

        device.beginPass(SHADOW_VIEW, shadowPass, &m_depthTarget);
        device.bindFrame(SHADOW_VIEW, lightFrame);

        for (const PreparedRenderPacket& packet : scene.opaque)
        {
            MeshDrawPacket shadowPacket = packet.draw;
            shadowPacket.material = &m_depthMaterial;

            device.submitMesh(SHADOW_VIEW, shadowPacket);
        }

        device.endPass();

        m_shadowData.lightViewProjection = lightFrame.projection * lightFrame.view;
        m_shadowData.depthTexture = &m_depthTarget.getDepthTexture();
        m_shadowData.lightIndex = shadowLightIndex;
    }

    bool ShadowRenderer::isValid() const
    {
        return m_resolution > 0 && m_depthTarget.isValid();
    }
}
