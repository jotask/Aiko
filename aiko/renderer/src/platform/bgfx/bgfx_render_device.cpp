#include "bgfx_render_device.h"

#include <math/math_vector.h>
#include <bgfx/platform.h>
#include <logger/logger.h>

#include "bgfx_platform_helper.h"
#include "display/display_manager.h"
#include "impl/bgfx_computebuffer_impl.h"
#include "impl/bgfx_computeshader_impl.h"
#include "impl/bgfx_texture_impl.h"

#if defined(AIKO_WINDOWS)
    #define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3native.h>
#elif defined(AIKO_LINUX)
    #define GLFW_EXPOSE_NATIVE_X11
    #include <GLFW/glfw3native.h>
#else
    #error OS unsupported!
#endif

#include <bgfx/bgfx.h>

#include <platform/bgfx/impl/bgfx_shader_impl.h>
#include <platform/bgfx/impl/bgfx_mesh_impl.h>
#include <platform/bgfx/impl/bgfx_framebuffer_impl.h>

namespace aiko::renderer::bgfx
{

    BgfxRenderDevice::BgfxRenderDevice()
        : m_boundShader(nullptr)
    {
        static_assert(sizeof(::bgfx::ViewId) == sizeof(ViewId),    "Bgfx ViewId type has changed");
        static_assert(std::is_same<::bgfx::ViewId, ViewId>::value, "Bgfx ViewId type has changed");
    }

    BgfxRenderDevice::~BgfxRenderDevice()
    {
    }

    bool BgfxRenderDevice::init(const DeviceInitDesc& desc)
    {

        AIKO_TODO("Enable render multi-thread only on release");
        ::bgfx::renderFrame();

        GLFWwindow* window = static_cast<GLFWwindow*>(desc.nativeWindowHandle);
        AIKO_ASSERT(window != nullptr, "Window is not GLFW?");

        ::bgfx::Init init;
        init.type = ::bgfx::RendererType::Count; // auto choose renderer (DirectX, OpenGL, etc.)
        #if defined(AIKO_WINDOWS)
            init.platformData.nwh = glfwGetWin32Window(window);
        #elif defined(AIKO_LINUX)


            init.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(window);
            init.platformData.ndt = glfwGetX11Display();
        #else
            #error OS unsupported!
        #endif
        init.resolution.width = desc.width;
        init.resolution.height = desc.height;
        init.resolution.reset = desc.vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE;

        if (::bgfx::init(init) == false)
        {
            logger::Log::error("Failed to init BGFX");
            return false;
        }

        // Log Init
        {

            static const std::unordered_map<uint32_t, std::string> vendorMap = {
                { 0x10DE, "NVIDIA" },
                { 0x1002, "AMD/ATI" },
                { 0x8086, "Intel" },
                { 0x5143, "Qualcomm" },
                { 0x13B5, "ARM" },
                { 0x5333, "S3 Graphics" },
                { 0x102B, "Matrox" }
                // add more if needed
            };

            auto getVendorName = [&](uint32_t vendorId) -> std::string
                {
                    auto it = vendorMap.find(vendorId);
                    if (it != vendorMap.end())
                        return it->second;
                    return std::string{ "Unknown" };
                };

            const uint32_t vendorId = ::bgfx::getCaps()->vendorId;
            const uint32_t deviceId = ::bgfx::getCaps()->deviceId;
            logger::Log::info() << "GPU: " << getVendorName(vendorId) << ", Vendor ID: 0x" << std::hex << vendorId << ", Device ID: 0x" << deviceId<< std::dec;
            logger::Log::info() << "BGFX Renderer: " << ::bgfx::getRendererName(::bgfx::getRendererType());
        }

        // ::bgfx::setDebug(BGFX_DEBUG_WIREFRAME | BGFX_DEBUG_STATS | BGFX_DEBUG_TEXT);

        m_csReadbackCopy.load("readback_copy.cs");

        return  true;

    }

    void BgfxRenderDevice::shutdown()
    {

    }

    void BgfxRenderDevice::resize(u32 width, u32 height, bool vsync)
    {
        m_width = width;
        m_height = height;
        ::bgfx::reset(width, height, vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE);
    }

    void BgfxRenderDevice::beginFrame()
    {
        // ::bgfx::setDebug(BGFX_DEBUG_STATS);
    }

    void BgfxRenderDevice::endFrame()
    {
        // NO op for now
    }

    void BgfxRenderDevice::beginPass(uint16_t viewId, const PassDescription& pass, FrameBuffer* frameBuffer)
    {

        if (frameBuffer == nullptr)
        {
            ::bgfx::setViewFrameBuffer(viewId, { ::bgfx::kInvalidHandle }); // backbuffer
        }
        else
        {
            BgfxFrameBufferImpl* fb = static_cast<BgfxFrameBufferImpl*>(frameBuffer->getImpl());
            AIKO_ASSERT(fb != nullptr, "Invalid FrameBuffer");
            AIKO_ASSERT(fb->isValid(), "Invalid FrameBuffer");
            ::bgfx::setViewFrameBuffer(viewId, fb->getFrameBufferHandler());
        }

        ::bgfx::setViewRect(viewId, 0, 0, (uint16_t)pass.width, (uint16_t)pass.height);

        uint16_t flags = 0;
        if (pass.clearColor) flags |= BGFX_CLEAR_COLOR;
        if (pass.clearDepth) flags |= BGFX_CLEAR_DEPTH;

        const uint32_t rgba = pass.clear.rgba();

        ::bgfx::setViewClear(viewId, flags, rgba, 1.0f, 0);
        ::bgfx::touch(viewId);

    }

    void BgfxRenderDevice::endPass()
    {
    }

    void BgfxRenderDevice::present()
    {
        // Start new job if none running
        if (!m_activeReadback && !m_readbackQueue.empty())
        {
            startReadbackInternal(m_readbackQueue.front());
            m_readbackQueue.pop_front();
        }

        // Process active job
        if (m_activeReadback)
        {
            auto& rb = *m_activeReadback;

            if (rb.stage == PendingReadback::Stage::SubmittedGPUWork)
            {
                dispatchPendingReadbackCopy();
                rb.framesSinceSubmit++;
            }
            else if (rb.stage == PendingReadback::Stage::ReadIssued)
            {
                rb.framesSinceSubmit++;
            }
        }

        ::bgfx::frame();
    }

    void BgfxRenderDevice::renderMesh(ViewId viewId, const mat4 world, const Mesh& mesh, const Material& material)
    {
        bindMaterial(material);
        drawMesh(viewId, world, mesh, material);
    }

    void BgfxRenderDevice::bindMaterial(const Material& material)
    {
        AIKO_ASSERT(material.m_shader.isValid(), "Invalid Shader");
        BgfxShaderImpl* shaderImpl = static_cast<BgfxShaderImpl*>(material.m_shader.getImpl());
        AIKO_ASSERT(shaderImpl != nullptr, "Material has not shader!");
        m_boundShader = shaderImpl;
    }

    void BgfxRenderDevice::drawMesh(ViewId viewId, const mat4& world, const Mesh& mesh, const Material& material)
    {
        AIKO_ASSERT(mesh.isValid(), "Invalid Mesh");
        auto* meshImpl = static_cast<BgfxMeshImpl*>(mesh.getImpl());
        AIKO_ASSERT(meshImpl != nullptr, "Mesh has no BGFX impl");

        const auto vb = meshImpl->getVertexBuffferHandler();
        const auto ib = meshImpl->getIndexBuffferHandler();

        if (::bgfx::isValid(vb) == false || ::bgfx::isValid(ib) == false)
        {
            // common cause: mesh never refreshed/uploaded
            logger::Log::warning("renderMesh: invalid VB/IB (did you call mesh.refresh/upload?)");
            return;
        }

        bindFrameUniforms();
        bindMaterialUniforms(material);

        ::bgfx::setTransform(world.data());
        ::bgfx::setVertexBuffer(0, vb);
        ::bgfx::setIndexBuffer(ib);
        ::bgfx::setState(s_default_state);

        AIKO_ASSERT(m_boundShader->isValid(), "Bound program not valid");

        ::bgfx::submit(viewId, m_boundShader->getProgramHandler());

    }

    void BgfxRenderDevice::presentFrameBufferToScreen(ViewId viewId, const ScreenFbo& screen)
    {

        const auto displaySize = DisplayManager::it().getDisplay()->getDisplaySize();

        FrameBuffer fb = screen.getFrameBuffer();

        AIKO_ASSERT(fb.isValid(), "Invalid framebuffer");

        BgfxFrameBufferImpl* fbo = static_cast<BgfxFrameBufferImpl*>(fb.getImpl());
        AIKO_ASSERT(fbo != nullptr, "Invalid FBO");

        BgfxTextureImpl* colorTexture = static_cast<BgfxTextureImpl*>(fb.getColorTexture().getImpl());
        AIKO_ASSERT(colorTexture != nullptr, "Invalid color texture for frame buffer");

        BgfxTextureImpl* depthTexture = static_cast<BgfxTextureImpl*>(fb.getDepthTexture().getImpl());
        AIKO_ASSERT(depthTexture != nullptr, "Invalid depth texture for frame buffer");

        BgfxShaderImpl* shaderImpl = static_cast<BgfxShaderImpl*>(screen.getMaterial().m_shader.getImpl());
        AIKO_ASSERT(shaderImpl != nullptr, "Screen program has no impl");

        BgfxMeshImpl* meshImpl = static_cast<BgfxMeshImpl*>(screen.getMesh().getImpl());
        AIKO_ASSERT(meshImpl != nullptr, "Mesh has no impl");

        ::bgfx::setViewFrameBuffer(viewId, {::bgfx::kInvalidHandle});

        ::bgfx::setViewRect(viewId, 0, 0, static_cast<uint16_t>(displaySize.x), static_cast<uint16_t>(displaySize.y));

        float view[16];
        float proj[16];
        bx::mtxIdentity(view);
        bx::mtxIdentity(proj);
        ::bgfx::setViewTransform(viewId, view, proj);

        ::bgfx::UniformHandle u_scene = shaderImpl->getUniformHandle("u_scene");
        AIKO_ASSERT(::bgfx::isValid(u_scene), "passthrough shader missing uniform u_scene");

        ::bgfx::setTexture(0, u_scene, colorTexture->getTextureHandler());

        AIKO_ASSERT(::bgfx::isValid(meshImpl->getVertexBuffferHandler()), "Invalid VB");
        AIKO_ASSERT(::bgfx::isValid(meshImpl->getIndexBuffferHandler()), "Invalid IB");

        ::bgfx::setVertexBuffer(0, meshImpl->getVertexBuffferHandler());
        ::bgfx::setIndexBuffer(meshImpl->getIndexBuffferHandler());

        // No depth test, no culling issues
        ::bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);

        // IMPORTANT: you still need a submit per draw
        ::bgfx::submit(viewId, shaderImpl->getProgramHandler());

    }

    void BgfxRenderDevice::presentTextureToScreen(ViewId viewId, const ScreenFbo& screen, const Texture& texture)
    {
        const auto displaySize = DisplayManager::it().getDisplay()->getDisplaySize();

        AIKO_ASSERT(texture.isValid(), "Invalid texture passed to presentTextureToScreen");

        BgfxTextureImpl* colorTexture = static_cast<BgfxTextureImpl*>(texture.getImpl());
        AIKO_ASSERT(colorTexture != nullptr, "Invalid texture impl");

        BgfxShaderImpl* shaderImpl = static_cast<BgfxShaderImpl*>(screen.getMaterial().m_shader.getImpl());
        AIKO_ASSERT(shaderImpl != nullptr, "Screen program has no impl");

        BgfxMeshImpl* meshImpl = static_cast<BgfxMeshImpl*>(screen.getMesh().getImpl());
        AIKO_ASSERT(meshImpl != nullptr, "Mesh has no impl");

        ::bgfx::setViewFrameBuffer(viewId, { ::bgfx::kInvalidHandle });

        ::bgfx::setViewRect(viewId, 0, 0,
            static_cast<uint16_t>(displaySize.x),
            static_cast<uint16_t>(displaySize.y));

        float view[16];
        float proj[16];
        bx::mtxIdentity(view);
        bx::mtxIdentity(proj);
        ::bgfx::setViewTransform(viewId, view, proj);

        ::bgfx::UniformHandle u_scene = shaderImpl->getUniformHandle("u_scene");
        AIKO_ASSERT(::bgfx::isValid(u_scene), "passthrough shader missing uniform u_scene");

        ::bgfx::setTexture(0, u_scene, colorTexture->getTextureHandler());

        AIKO_ASSERT(::bgfx::isValid(meshImpl->getVertexBuffferHandler()), "Invalid VB");
        AIKO_ASSERT(::bgfx::isValid(meshImpl->getIndexBuffferHandler()), "Invalid IB");

        ::bgfx::setVertexBuffer(0, meshImpl->getVertexBuffferHandler());
        ::bgfx::setIndexBuffer(meshImpl->getIndexBuffferHandler());

        ::bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);

        ::bgfx::submit(viewId, shaderImpl->getProgramHandler());
    }

    void BgfxRenderDevice::drawMeshInstanced(ViewId viewId, const Mesh& mesh, const Material& material, const void* data, u32 instanceCount, u32 instanceStrideBytes)
    {
        bindMaterial(material);

        auto* meshImpl = static_cast<BgfxMeshImpl*>(mesh.getImpl());
        AIKO_ASSERT(meshImpl, "Mesh backend not implemented");

        const auto vb = meshImpl->getVertexBuffferHandler();
        const auto ib = meshImpl->getIndexBuffferHandler();
        AIKO_ASSERT(::bgfx::isValid(vb) && ::bgfx::isValid(ib), "Invalid VB/IB");

        // bgfx requirement: stride must be multiple of 16 bytes
        AIKO_ASSERT((instanceStrideBytes % 16u) == 0u, "Instance stride must be multiple of 16 bytes");

        bindFrameUniforms();
        bindMaterialUniforms(material);

        ::bgfx::InstanceDataBuffer idb;
        ::bgfx::allocInstanceDataBuffer(&idb, instanceCount, instanceStrideBytes);
        memcpy(idb.data, data, instanceCount * instanceStrideBytes);

        ::bgfx::setVertexBuffer(0, vb);
        ::bgfx::setIndexBuffer(ib);
        ::bgfx::setInstanceDataBuffer(&idb);

        if (material.m_gpuInstanceBuffer) // or however you expose it
        {
            auto* bufImpl =
                static_cast<BgfxComputeBufferImpl*>(
                    material.m_gpuInstanceBuffer->getImpl());

            AIKO_ASSERT(bufImpl && bufImpl->isValid(), "Invalid GPU instance buffer");

            ::bgfx::setBuffer(
                7,                      // MUST match BUFFER_RO(...,7)
                bufImpl->handle(),
                ::bgfx::Access::Read);
        }

        ::bgfx::setState(s_default_state);

        ::bgfx::submit(viewId, m_boundShader->getProgramHandler());

    }

    void BgfxRenderDevice::bindFrame(ViewId viewId, const FrameData& u)
    {
        m_frameData = u;
        ::bgfx::setViewTransform(viewId, u.view.data(), u.projection.data() );
    }

    void BgfxRenderDevice::execute(ViewId viewId, const ComputePass& pass)
    {
        if (!pass.shader)
            return;

        auto* csImpl = static_cast<BgfxComputeShaderImpl*>(pass.shader->getImpl());
        if (!csImpl || !csImpl->isValid())
            return;

        // Bind images
        for (const ComputeImageBinding& b : pass.images)
        {
            if (!b.texture) continue;
            auto* texImpl = static_cast<BgfxTextureImpl*>(b.texture->getImpl());
            AIKO_ASSERT(texImpl && texImpl->isValid(), "Invalid compute image texture");

            ::bgfx::setImage(b.stage,
                             texImpl->getTextureHandler(),
                             0,
                             toBgfxAccess(b.access));
        }

        // Bind buffers
        for (const ComputeBufferBinding& b : pass.buffers)
        {
            if (!b.buffer) continue;
            auto* bufImpl = static_cast<BgfxComputeBufferImpl*>(b.buffer->getImpl());
            AIKO_ASSERT(bufImpl && bufImpl->isValid(), "Invalid compute buffer");

            ::bgfx::setBuffer(b.stage,
                              bufImpl->handle(),
                              toBgfxAccess(b.access));
        }

        // Uniforms
        for (const ComputeVec4Uniform& u : pass.vec4Uniforms)
        {
            if (!u.name) continue;
            ::bgfx::UniformHandle h = csImpl->getUniformHandle(u.name);
            if (::bgfx::isValid(h))
                ::bgfx::setUniform(h, &u.value);
        }

        // Dispatch
        if (pass.dispatch.groupsX == 0 || pass.dispatch.groupsY == 0 || pass.dispatch.groupsZ == 0)
            return;

        ::bgfx::dispatch(viewId,
                         csImpl->getProgramHandler(),
                         pass.dispatch.groupsX,
                         pass.dispatch.groupsY,
                         pass.dispatch.groupsZ);

    }

    void BgfxRenderDevice::requestReadback(const ComputeReadbackRequest& request)
    {

        if (!request.buffer || request.byteSize == 0)
            return;

        ComputeReadbackRequest r = request;
        if (r.id == 0)
            r.id = m_nextReadbackId++;

        m_readbackQueue.push_back(r);
    }

    bool BgfxRenderDevice::pollReadback(ComputeReadbackResult& result)
    {
        if (!m_completedReadbacks.empty())
        {
            result = std::move(m_completedReadbacks.front());
            m_completedReadbacks.pop_front();
            return true;
        }

        if (!m_activeReadback)
            return false;

        auto& rb = *m_activeReadback;

        if (rb.stage == PendingReadback::Stage::SubmittedGPUWork)
        {
            if (rb.framesSinceSubmit < 2)
                return false;

            ::bgfx::readTexture(rb.readTex, rb.cpu.data());

            rb.stage = PendingReadback::Stage::ReadIssued;
            rb.framesSinceSubmit = 0;
            return false;
        }

        if (rb.stage == PendingReadback::Stage::ReadIssued)
        {
            if (rb.framesSinceSubmit < 2)
                return false;

            uint32_t bytes = ::bgfx::readTexture(rb.readTex, rb.cpu.data());
            if (bytes == 0)
                return false;

            ComputeReadbackResult r;
            r.id = rb.id;
            r.ready = true;
            r.data = std::move(rb.cpu);

            m_completedReadbacks.push_back(std::move(r));

            cleanupReadback(rb);

            m_activeReadback.reset();

            return false;
        }

        return false;
    }

    void BgfxRenderDevice::drawMeshInstancedGpu(ViewId viewId, const GpuInstanceDrawDesc& desc)
    {

        auto* meshImpl = static_cast<BgfxMeshImpl*>(desc.mesh->getImpl());
        auto* shaderImpl = static_cast<BgfxShaderImpl*>(desc.material->m_shader.getImpl());
        auto* bufferImpl = static_cast<BgfxComputeBufferImpl*>(desc.instanceBuffer->getImpl());

        AIKO_ASSERT(meshImpl != nullptr, "Invalid handler");
        AIKO_ASSERT(shaderImpl != nullptr, "Invalid handler");
        AIKO_ASSERT(bufferImpl != nullptr, "Invalid handler");

        bindMaterial(*desc.material);
        bindFrameUniforms();
        bindMaterialUniforms(*desc.material);

        float mtx[16];
        bx::mtxIdentity(mtx);
        ::bgfx::setTransform(mtx);

        ::bgfx::setBuffer( 7, bufferImpl->handle(), ::bgfx::Access::Read );

        ::bgfx::setVertexBuffer(0, meshImpl->getVertexBuffferHandler());
        ::bgfx::setIndexBuffer(meshImpl->getIndexBuffferHandler());

        ::bgfx::setState(s_default_state);

        ::bgfx::setInstanceCount(desc.instanceCount);
        ::bgfx::submit(viewId, shaderImpl->getProgramHandler());

    }

    void BgfxRenderDevice::dispatchPendingReadbackCopy()
    {

        constexpr ViewId READBACK_VIEW = 250;

        if (!m_activeReadback)
            return;

        auto& rb = *m_activeReadback;

        AIKO_ASSERT(rb.source != nullptr, "Readback source missing");
        AIKO_ASSERT(::bgfx::isValid(rb.computeTex), "Readback computeTex invalid");
        AIKO_ASSERT(::bgfx::isValid(rb.readTex), "Readback readTex invalid");

        auto* srcImpl = static_cast<BgfxComputeBufferImpl*>(rb.source->getImpl());
        AIKO_ASSERT(srcImpl && srcImpl->isValid(), "Invalid readback source buffer");

        auto* csImpl = static_cast<BgfxComputeShaderImpl*>(m_csReadbackCopy.getImpl());
        AIKO_ASSERT(csImpl && csImpl->isValid(), "Invalid readback copy shader");

        const uint32_t vec4Count = rb.byteSize / 16u;
        const uint32_t gx = (vec4Count + 63u) / 64u;

        // Setup view
        ::bgfx::setViewRect(READBACK_VIEW, 0, 0, 1, 1);
        ::bgfx::touch(READBACK_VIEW);

        // Bind src buffer and dst computeTex
        ::bgfx::setBuffer(0, srcImpl->handle(), ::bgfx::Access::Read);
        ::bgfx::setImage(1, rb.computeTex, 0, ::bgfx::Access::Write);

        // u_params.x = vec4Count
        const vec4 params(float(vec4Count), 0.0f, 0.0f, 0.0f);
        ::bgfx::UniformHandle u = csImpl->getUniformHandle("u_params");
        AIKO_ASSERT(::bgfx::isValid(u), "readback_copy.cs missing u_params");
        ::bgfx::setUniform(u, &params);

        // Dispatch copy shader
        ::bgfx::dispatch(READBACK_VIEW, csImpl->getProgramHandler(), gx, 1, 1);

        // Blit computeTex -> readTex
        ::bgfx::blit(
            READBACK_VIEW,
            rb.readTex, 0, 0,
            rb.computeTex, 0, 0,
            (uint16_t)vec4Count, 1
        );

    }

    void BgfxRenderDevice::startReadbackInternal(const ComputeReadbackRequest& request)
    {
        if (m_activeReadback.has_value())
            return;

        if (!request.buffer || request.byteSize == 0)
            return;

        AIKO_ASSERT((request.byteSize % 16u) == 0u, "Readback byteSize must be multiple of 16 (vec4)");

        m_activeReadback.emplace();
        auto& rb = *m_activeReadback;

        rb.id = request.id;
        rb.source = request.buffer;
        rb.byteSize = request.byteSize;
        rb.cpu.assign(request.byteSize, 0);

        const uint32_t vec4Count = request.byteSize / 16u;

        rb.computeTex = ::bgfx::createTexture2D(
            (uint16_t)vec4Count, 1, false, 1,
            ::bgfx::TextureFormat::RGBA32F,
            BGFX_TEXTURE_COMPUTE_WRITE
        );

        rb.readTex = ::bgfx::createTexture2D(
            (uint16_t)vec4Count, 1, false, 1,
            ::bgfx::TextureFormat::RGBA32F,
            BGFX_TEXTURE_READ_BACK | BGFX_TEXTURE_BLIT_DST
        );

        AIKO_ASSERT(::bgfx::isValid(rb.computeTex), "Failed to create computeTex for readback");
        AIKO_ASSERT(::bgfx::isValid(rb.readTex), "Failed to create readTex for readback");

        rb.stage = PendingReadback::Stage::SubmittedGPUWork;
        rb.framesSinceSubmit = 0;

    }

    void BgfxRenderDevice::cleanupReadback(PendingReadback& rb)
    {
        if (::bgfx::isValid(rb.computeTex))
            ::bgfx::destroy(rb.computeTex);

        if (::bgfx::isValid(rb.readTex))
            ::bgfx::destroy(rb.readTex);

        rb.computeTex = { ::bgfx::kInvalidHandle };
        rb.readTex    = { ::bgfx::kInvalidHandle };
    }

    void BgfxRenderDevice::bindFrameUniforms()
    {
        if (m_boundShader == nullptr)
        {
            return;
        }

        if (m_boundShader->hasUniform("u_cameraPos"))
        {
            m_boundShader->setVec3("u_cameraPos", m_frameData.cameraPosition);
        }

        if (m_boundShader->hasUniform("u_ambientColor"))
        {
            m_boundShader->setVec4("u_ambientColor", m_frameData.ambient.color.toVec4());
        }

        if (m_boundShader->hasUniform("u_ambientIntensity"))
        {
            m_boundShader->setFloat("u_ambientIntensity", m_frameData.ambient.intensity);
        }

        if (m_boundShader->hasUniform("u_lightType") &&
            m_boundShader->hasUniform("u_lightPosRange") &&
            m_boundShader->hasUniform("u_lightDir") &&
            m_boundShader->hasUniform("u_lightColorInt") &&
            m_boundShader->hasUniform("u_lightSpotCos"))
        {
            vec4 type[MAX_LIGHTS] = {};
            vec4 posRange[MAX_LIGHTS] = {};
            vec4 dir[MAX_LIGHTS] = {};
            vec4 colorInt[MAX_LIGHTS] = {};
            vec4 spot[MAX_LIGHTS] = {};

            const int count = std::min(static_cast<int>(m_frameData.lights.size()), static_cast<int>(MAX_LIGHTS));

            // also upload lightCount if shader expects it
            if (m_boundShader->hasUniform("u_lightCount"))
            {
                m_boundShader->setVec4("u_lightCount", vec4(static_cast<float>(count), 0, 0, 0));
            }

            for (int i = 0; i < count; ++i)
            {
                const LightData& l = m_frameData.lights.at(i);
                type[i]     = vec4(static_cast<float>(l.type), 0, 0, 0);
                posRange[i] = vec4(l.position.x, l.position.y, l.position.z, l.range);
                dir[i]      = vec4(l.direction.x, l.direction.y, l.direction.z, 0);
                colorInt[i] = vec4(l.color.r, l.color.g, l.color.b, l.intensity);
                spot[i]     = vec4(l.innerCos, l.outerCos, 0, 0);
            }

            m_boundShader->setVec4Array("u_lightType", type, count);
            m_boundShader->setVec4Array("u_lightPosRange", posRange, count);
            m_boundShader->setVec4Array("u_lightDir", dir, count);
            m_boundShader->setVec4Array("u_lightColorInt", colorInt, count);
            m_boundShader->setVec4Array("u_lightSpotCos", spot, count);
        }

    }

    void BgfxRenderDevice::bindMaterialUniforms(const Material& material)
    {

        if (m_boundShader == nullptr) return;

        // u_baseColor
        m_boundShader->setVec4("u_baseColor", material.m_baseColor.toVec4());

        // flags
        const bool hasTexture = material.m_diffuse.isValid();
        m_boundShader->setVec4("u_flags", vec4(
                hasTexture == true ? 1.0f : 0.0f,
                material.m_userVertexColor == true ? 1.0f : 0.0f,
                material.m_lit == true ? 1.0f : 0.0f,
                0.0f
            ));

        if (hasTexture == true)
        {
            // sampler name in model.fs is u_texture
            ::bgfx::UniformHandle s_tex = m_boundShader->getUniformHandle("u_texture");
            if (::bgfx::isValid(s_tex) == true)
            {
                auto* texImpl = static_cast<BgfxTextureImpl*>(material.m_diffuse.getImpl());
                if (texImpl && texImpl->isValid() == true)
                {
                    ::bgfx::setTexture(0, s_tex, texImpl->getTextureHandler(), texImpl->getSamplerFlags());
                }
            }
        }

    }

}
