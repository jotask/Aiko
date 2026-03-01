#include "render_module.h"

#include "modules/module_connector.h"

#include <aiko_renderer.h>

#include "display/display_manager.h"
#include "models/camera.h"
#include "time/time.h"

namespace aiko
{
    RenderModule::RenderModule(Aiko* aiko)
        : BaseModule(aiko)
        , m_mainCamera(nullptr)
    {
    }

    void RenderModule::setMainCamera(const Camera* camera)
    {
        m_mainCamera = camera;
    }

    void RenderModule::submitLights(const AmbientLight& ambient, const std::vector<LightData>& data)
    {
        AikoRenderer::it().submit(ambient, data);
    }

    void RenderModule::init()
    {
        AikoRenderer::it().init();
        // TEMP compute validation
        {
            auto size = DisplayManager::it().getDisplay()->getDisplaySize();

            // output texture for compute (matches IMAGE2D_WO(u_output, rgba8, 2))
            if (m_debugOut.isValid())
                m_debugOut.unload();

            m_debugOut.create({
                .type = texture::TextureType::Sampled,
                .format = texture::TextureFormat::RGBA8,
                .width = size.x,
                .height = size.y,
                .mipmaps = false,
                .computeWrite = true
            });

            m_particlesCS.load("particles.cs");

            std::vector<vec4> pos(m_particleCount);
            std::vector<vec4> vel(m_particleCount);

            for (u32 i = 0; i < m_particleCount; ++i)
            {
                float t = float(i) / float(m_particleCount);
                float a = t * 6.2831853f;
                float r = 0.25f;

                pos[i] = vec4(std::cos(a) * r, std::sin(a) * r, 0.0f, 1.0f);
                vel[i] = vec4(-std::sin(a) * 0.2f, std::cos(a) * 0.2f, 0.0f, 0.0f);
            }

            m_posBuffer.createVec4(m_particleCount, pos.data());
            m_velBuffer.createVec4(m_particleCount, vel.data());

            m_computeInit = true;
        }
    }

    void RenderModule::update()
    {
        BaseModule::update();
    }

    void RenderModule::render()
    {
        if (!m_computeInit)
            return;

        ComputePass pass;
        pass.shader = &m_particlesCS;

        // stage 0/1 match shader BUFFER_RW(u_pos...0) and BUFFER_RW(u_vel...1)
        pass.buffers.push_back({ 0, &m_posBuffer, ComputeAccess::ReadWrite });
        pass.buffers.push_back({ 1, &m_velBuffer, ComputeAccess::ReadWrite });

        // stage 2 matches IMAGE2D_WO(u_output, rgba8, 2)
        pass.images.push_back({ 2, &m_debugOut, ComputeAccess::Write });

        const float dt = Time::it().getDeltaTime();
        pass.vec4Uniforms.push_back({ "u_params", vec4(dt, float(m_particleCount), 0.0f, 0.0f) });

        pass.dispatch.groupsX = (m_particleCount + 63) / 64;
        pass.dispatch.groupsY = 1;
        pass.dispatch.groupsZ = 1;

        AikoRenderer::it().enqueueCompute(pass);

        // Request readback once per second (TEMP debug)
        // TEMP: request a readback once, then request again only after we got a result
        if (!m_readbackRequested)
        {
            ComputeReadbackRequest req;
            req.buffer = &m_posBuffer;
            req.byteSize = 16 * sizeof(vec4); // first 16 vec4s
            AikoRenderer::it().requestReadback(req);

            m_readbackRequested = true;
        }

        if (m_readbackRequested)
        {
            ComputeReadbackResult res;
            if (AikoRenderer::it().pollReadback(res) && res.ready)
            {
                const vec4* v = reinterpret_cast<const vec4*>(res.data.data());
                logger::Log::info("pos0=(%f,%f,%f,%f) pos1=(%f,%f,%f,%f)",
                    v[0].x,v[0].y,v[0].z,v[0].w,
                    v[1].x,v[1].y,v[1].z,v[1].w);

                // request next one (optional)
                m_readbackRequested = false;
            }
        }

        // NOTE: no particle draw here (compute-only validation).
        // If you want to see it, present m_debugOut in AikoRenderer's screen pass.
        AikoRenderer::it().setDebugTexture(&m_debugOut);



    }

    void RenderModule::beginFrame()
    {
        m_instances.clear();
        AikoRenderer::it().beginFrame();
    }

    void RenderModule::endFrame()
    {
        AIKO_ASSERT(m_mainCamera != nullptr, "Main camera not set. Forgot to call set Main camera?");
        AikoRenderer::it().render(*m_mainCamera);
        AikoRenderer::it().endFrame();
        m_instances.clear();
    }

    void RenderModule::dispose()
    {
        AikoRenderer::it().dispose();
    }

}
