#pragma once

#include "models/frame_buffer.h"
#include "models/material.h"
#include "models/texture.h"

#include "types/aiko_renderer_types.h"
#include "renderer/frame_uniforms.h"
#include "renderer/shadow/shadow_data.h"

#include <array>

namespace aiko
{

    class IAssetRegistry;

    namespace renderer
    {

        class IRenderDevice;

        class ShadowRenderer
        {
        public:
            ShadowRenderer() = default;
            ~ShadowRenderer() = default;

            ShadowRenderer(const ShadowRenderer&) = delete;
            ShadowRenderer& operator=(const ShadowRenderer&) = delete;

            void init(IAssetRegistry& registry, u32 resolution = 1024);
            void dispose();

            void render(IRenderDevice& device, const FrameData& sceneFrame, const PreparedScenePass& scene);

            bool isValid() const;

            const vector<ShadowData>& shadowData() const { return m_shadowData; }

        private:
            struct ShadowTarget
            {
                FrameBuffer framebuffer;
            };

            struct PointShadowTarget
            {
                Texture depthCubemap;
                std::array<FrameBuffer, 6> faces;
            };

            // Existing directional and spotlight shadow targets.
            std::array<ShadowTarget, MaxFrameLights> m_targets;

            // One cubemap per point-light slot, created on demand.
            std::array<PointShadowTarget, MaxFrameLights> m_pointTargets;

            vector<ShadowData> m_shadowData;

            u32 m_resolution = 0;

            Material m_depthMaterial;
        };
    }
}
