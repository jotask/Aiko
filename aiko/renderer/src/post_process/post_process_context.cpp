
#include "post_process_context.h"

#include "renderer/Irenderdevice.h"

namespace aiko
{
    PostProcessContext::PostProcessContext(renderer::IRenderDevice& device, std::deque<Material>& frameMaterials, const Texture& input, const Texture& sceneDepth, const RenderTarget& output)
        : m_device(device)
        , m_frameMaterials(frameMaterials)
        , m_input(input)
        , m_sceneDepth(sceneDepth)
        , m_output(output)
    {
        AIKO_ASSERT(input.isValid(), "Invalid post-process input");
        AIKO_ASSERT(sceneDepth.isValid(), "Invalid scene depth");
        AIKO_ASSERT(output.isValid(), "Invalid post-process output");
        AIKO_ASSERT(&input != &output.colorTexture(), "Post-process input and output must differ"
        );
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

    void PostProcessContext::renderFullscreen(const Material& material)
    {
        AIKO_ASSERT(material.m_shaderId != InvalidAssetId, "Post-process material requires a shader");

        const ivec2 size = m_output.size();

        AIKO_ASSERT(size.x > 0 && size.y > 0, "Invalid post-process output dimensions");

        m_frameMaterials.push_back(material);

        Material& staged = m_frameMaterials.back();
        staged.setTexture("u_texture", &m_input);

        AIKO_ASSERT(!staged.m_renderState.depthTest && !staged.m_renderState.depthWrite, "Fullscreen post-process materials must disable depth testing and writing");

        for (const auto& [name, binding] : staged.textureBindings())
        {
            AIKO_UNUSED(name);
            AIKO_ASSERT(binding.runtimeTexture != &m_output.colorTexture(), "Post-process material samples its output attachment");
        }

        const renderer::PassDescription pass{
            .width = static_cast<u32>(size.x),
            .height = static_cast<u32>(size.y),
            .colorLoadOp = renderer::AttachmentLoadOp::Clear,
            .depthLoadOp = renderer::AttachmentLoadOp::DontCare
        };

        const renderer::FrameData frame{};

        m_device.prepareMaterial(staged);

        m_device.beginPass(POST_PROCESS_VIEW, pass, &m_output.frameBuffer());

        m_device.bindFrame(POST_PROCESS_VIEW, frame);
        m_device.submitFullscreen(POST_PROCESS_VIEW, staged);
        m_device.endPass();
    }
}
