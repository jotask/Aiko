
#pragma once

#include <deque>
#include <functional>

#include "post_process_effect.h"
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

        using ScratchAllocator = std::function<RenderTarget&(u32 slot, u32 width, u32 height, PostProcessDomain domain)>;

        PostProcessContext(renderer::IRenderDevice& device, std::deque<Material>& frameMaterials, const Texture& input, const Texture& sceneDepth, const RenderTarget& output, ScratchAllocator scratchAllocator);

        PostProcessContext(const PostProcessContext&) = delete;
        PostProcessContext& operator=(const PostProcessContext&) = delete;

        const Texture& inputTexture() const;
        const Texture& sceneDepth() const;
        const RenderTarget& outputTarget() const;

        RenderTarget& scratch(u32 slot, u32 width, u32 height, PostProcessDomain domain);

        void renderFullscreen(const Material& material, const Texture& input, const RenderTarget& output);
        void renderFullscreen(const Material& material);

    private:
        renderer::IRenderDevice& m_device;
        std::deque<Material>& m_frameMaterials;

        const Texture& m_input;
        const Texture& m_sceneDepth;
        const RenderTarget& m_output;

        ScratchAllocator m_scratchAllocator;
    };
}
