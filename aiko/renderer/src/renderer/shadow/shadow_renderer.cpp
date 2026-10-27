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

        for (ShadowTarget& target : m_targets)
        {
            target.framebuffer.create({
                .width = static_cast<int>(resolution),
                .height = static_cast<int>(resolution),
                .colorFormat = TextureFormat::None,
            });

            AIKO_ASSERT(target.framebuffer.isValid(), "Failed to initialize shadow depth target");
        }

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
        m_shadowData.clear();
        for (ShadowTarget& target : m_targets)
        {
            target.framebuffer.unload();
        }
        m_resolution = 0;
    }

    void ShadowRenderer::render(IRenderDevice& device, const FrameData& sceneFrame, const PreparedScenePass& scene)
    {
        AIKO_ASSERT(isValid(), "ShadowRenderer is not initialized");

        m_shadowData.clear();

        const u32 lightCount = static_cast<u32>(std::min<size_t>(sceneFrame.lights.size(), MaxFrameLights));

        const PassDescription shadowPass{
            .width = m_resolution,
            .height = m_resolution,
            .colorLoadOp = AttachmentLoadOp::DontCare,
            .depthLoadOp = AttachmentLoadOp::Clear,
        };

        device.prepareMaterial(m_depthMaterial);

        for (u32 i = 0; i < lightCount; ++i)
        {
            const LightData& light = sceneFrame.lights[i];

            if (light.intensity <= 0.0f || !light.castsShadows)
            {
                continue;
            }

            if (light.type != LightType::Directional && light.type != LightType::Spot)
            {
                continue;
            }

            AIKO_ASSERT(math::isfinite(light.direction) && math::length(light.direction) > 0.0001f, "Shadow light direction must be finite and nonzero");

            const vec3 direction = math::normalize(light.direction);

            const vec3 up = std::abs(direction.y) > 0.99f
                ? vec3(0.0f, 0.0f, 1.0f)
                : vec3(0.0f, 1.0f, 0.0f);

            FrameData lightFrame{};

            if (light.type == LightType::Directional)
            {
                constexpr float shadowDistance = 40.0f;
                constexpr float shadowExtent = 30.0f;

                const vec3 center = sceneFrame.cameraPosition;
                const vec3 eye = center - direction * shadowDistance;

                lightFrame.view = math::lookAt(eye, center, up);

                lightFrame.projection = math::ortho(
                    -shadowExtent,
                     shadowExtent,
                    -shadowExtent,
                     shadowExtent,
                     0.1f,
                     shadowDistance * 2.0f
                );
            }
            else
            {
                AIKO_ASSERT(math::isfinite(light.position), "Spotlight position must be finite");
                AIKO_ASSERT(light.range > 0.1f, "Spotlight range must exceed the shadow near plane");
                AIKO_ASSERT(light.outerCos > 0.0f && light.outerCos < 1.0f, "Invalid spotlight outer cone");

                lightFrame.view = math::lookAt(light.position, light.position + direction, up);

                const float fov = 2.0f * math::degrees(std::acos(light.outerCos));

                lightFrame.projection = math::perspective(fov, 1.0f, 0.1f, light.range);
            }

            // One persistent depth target per eligible shadow light.
            ShadowTarget& target = m_targets[m_shadowData.size()];

            device.beginPass(SHADOW_VIEW, shadowPass, &target.framebuffer);
            device.bindFrame(SHADOW_VIEW, lightFrame);

            for (const PreparedRenderPacket& packet : scene.opaque)
            {
                MeshDrawPacket shadowPacket = packet.draw;
                shadowPacket.material = &m_depthMaterial;

                device.submitMesh(SHADOW_VIEW, shadowPacket);
            }

            device.endPass();

            m_shadowData.push_back({
                .lightViewProjection = lightFrame.projection * lightFrame.view,
                .depthTexture = &target.framebuffer.getDepthTexture(),
                .lightIndex = i,
            });
        }
    }

    bool ShadowRenderer::isValid() const
    {
        return m_resolution > 0 &&
            std::all_of(
                m_targets.begin(),
                m_targets.end(),
                [](const ShadowTarget& target) { return target.framebuffer.isValid(); }
            );
    }
}
