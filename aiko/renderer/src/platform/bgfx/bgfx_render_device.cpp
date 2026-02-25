#include "bgfx_render_device.h"

#include <math/math_vector.h>
#include <bgfx/platform.h>
#include <logger/logger.h>

#include "display/display_manager.h"
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
        ::bgfx::frame();
    }

    void BgfxRenderDevice::setViewTransform(ViewId viewId, const mat4& view, const mat4& projection)
    {
        ::bgfx::setViewTransform(viewId, view.data(), projection.data() );
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

        // UNIFORMS
        // bgfx needs to set uniforms per draw
        {

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

            if (material.m_lit == true)
            {
                m_boundShader->setVec4("u_lightDir",   vec4(1,1,1,0));
                m_boundShader->setVec4("u_lightColor", vec4(1,1,1,1));
                m_boundShader->setFloat("u_ambient", 0.35f);
            }

            if (hasTexture == true)
            {
                // sampler name in model.fs is u_texture
                ::bgfx::UniformHandle s_tex = m_boundShader->getUniformHandle("u_texture");
                if (::bgfx::isValid(s_tex) == true)
                {
                    auto* texImpl = static_cast<BgfxTextureImpl*>(material.m_diffuse.getImpl());
                    if (texImpl && texImpl->isValid() == true)
                    {
                        ::bgfx::setTexture(0, s_tex, texImpl->getTextureHandler());
                    }
                }
            }

        }

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

    void BgfxRenderDevice::drawMeshInstanced(ViewId viewId, const Mesh& mesh, const Material& material, const void* data, u32 instanceCount, u32 instanceStrideBytes)
    {
        bindMaterial(material);

        AIKO_ASSERT(mesh.isValid(), "Invalid Mesh");
        BgfxMeshImpl* meshImpl = static_cast<BgfxMeshImpl*>(mesh.getImpl());
        AIKO_ASSERT(meshImpl != nullptr, "Mesh backend not implemented");

        // TODO bind uniforms

        ::bgfx::InstanceDataBuffer buff;
        ::bgfx::allocInstanceDataBuffer(&buff, instanceCount, instanceStrideBytes);
        memcpy(buff.data, data, instanceCount * instanceStrideBytes);

        ::bgfx::setInstanceDataBuffer(&buff);

        ::bgfx::setState(s_default_state);

        ::bgfx::submit(viewId, m_boundShader->getProgramHandler());


    }
}
