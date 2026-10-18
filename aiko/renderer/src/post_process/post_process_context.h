
#pragma once

#include <deque>

#include "models/material.h"
#include "models/render_target.h"

namespace aiko::renderer
{
    class IRenderDevice;
}

namespace aiko
{
    class PostProcessContext final
    {
    public:
        PostProcessContext(renderer::IRenderDevice& device, std::deque<Material>& frameMaterials, const Texture& input, const Texture& sceneDepth, const RenderTarget& output);

        PostProcessContext(const PostProcessContext&) = delete;
        PostProcessContext& operator=(const PostProcessContext&) = delete;

        const Texture& inputTexture() const;
        const Texture& sceneDepth() const;
        const RenderTarget& outputTarget() const;

        void renderFullscreen(const Material& material);

    private:
        renderer::IRenderDevice& m_device;
        std::deque<Material>& m_frameMaterials;

        const Texture& m_input;
        const Texture& m_sceneDepth;
        const RenderTarget& m_output;
    };
}
