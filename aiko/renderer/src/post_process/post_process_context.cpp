
#include "post_process_context.h"

#include "renderer/Irenderdevice.h"

namespace aiko
{
    PostProcessContext::PostProcessContext(renderer::IRenderDevice& device, std::deque<Material>& frameMaterials, const Texture& input, const Texture& sceneDepth, const RenderTarget& output, ScratchAllocator scratchAllocator)
        : m_device(device)
        , m_frameMaterials(frameMaterials)
        , m_input(input)
        , m_sceneDepth(sceneDepth)
        , m_output(output)
        , m_scratchAllocator(std::move(scratchAllocator))
    {
        AIKO_ASSERT(input.isValid(), "Invalid post-process input");
        AIKO_ASSERT(sceneDepth.isValid(), "Invalid scene depth");
        AIKO_ASSERT(output.isValid(), "Invalid post-process output");
        AIKO_ASSERT(&input != &output.colorTexture(), "Post-process input and output must differ");
        AIKO_ASSERT(static_cast<bool>(m_scratchAllocator), "Post-process scratch allocator is missing");
    }

    const Texture& PostProcessContext::inputTexture() const
    {
        return m_input;
    }

    const Texture& PostProcessContext::sceneDepth() const
    {
        return m_sceneDepth;
    }

    const RenderTarget& PostProcessContext::outputTarget() const
    {
        return m_output;
    }
    RenderTarget& PostProcessContext::scratch(u32 slot, u32 width, u32 height, PostProcessDomain domain)
    {
        AIKO_ASSERT(width > 0 && height > 0, "Invalid scratch dimensions");
        RenderTarget& target = m_scratchAllocator(slot, width, height, domain);

        AIKO_ASSERT(target.isValid(), "Scratch target is invalid");
        AIKO_ASSERT(&target.colorTexture() != &m_input, "Scratch target aliases effect input");
        AIKO_ASSERT(&target.colorTexture() != &m_output.colorTexture(), "Scratch target aliases effect output");

        return target;
    }

    void PostProcessContext::renderFullscreen(const Material& material, const Texture& input, const RenderTarget& output)
    {
        AIKO_ASSERT(input.isValid(), "Invalid post-process input");
        AIKO_ASSERT(output.isValid(), "Invalid post-process output");
        AIKO_ASSERT(&input != &output.colorTexture(), "Post-process pass cannot sample its output");
        AIKO_ASSERT(material.m_shaderId != InvalidAssetId, "Post-process material requires a shader");
        AIKO_ASSERT(!material.m_renderState.depthTest && !material.m_renderState.depthWrite, "Post-process materials must disable depth testing and writing");

        const ivec2 size = output.size();

        AIKO_ASSERT(size.x > 0 && size.y > 0, "Invalid output dimensions");

        m_frameMaterials.push_back(material);

        Material& staged = m_frameMaterials.back();
        SamplerState sampler{};
        sampler.wrapU = TextureWrapMode::Clamp;
        sampler.wrapV = TextureWrapMode::Clamp;

        staged.setTexture("u_texture", &input, sampler);

        for (const auto& [name, binding] : staged.textureBindings())
        {
            AIKO_UNUSED(name);
            AIKO_ASSERT(binding.runtimeTexture != &output.colorTexture(), "Post-process material samples its output attachment");
        }

        const renderer::PassDescription pass{
            .width = static_cast<u32>(size.x),
            .height = static_cast<u32>(size.y),
            .colorLoadOp = renderer::AttachmentLoadOp::Clear,
            .depthLoadOp = renderer::AttachmentLoadOp::DontCare
        };

        const renderer::FrameData frame{};

        m_device.prepareMaterial(staged);

        m_device.beginPass(POST_PROCESS_VIEW, pass, &output.frameBuffer());

        m_device.bindFrame(POST_PROCESS_VIEW, frame);
        m_device.submitFullscreen(POST_PROCESS_VIEW, staged);
        m_device.endPass();
    }

    void PostProcessContext::renderFullscreen(const Material& material)
    {
        renderFullscreen(material, m_input, m_output);
    }
}
