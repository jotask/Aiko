#pragma once

#include "models/frame_buffer.h"
#include "models/material.h"

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

            std::array<ShadowTarget, MaxFrameLights> m_targets;
            vector<ShadowData> m_shadowData;

            u32 m_resolution = 0;

            Material m_depthMaterial;
        };
    }
}
