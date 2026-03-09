 #pragma once

#include <aiko_types.h>
#include <math/math.h>

#include "texture.h"
#include "frame_buffer.h"
#include "material.h"
#include "mesh.h"

 namespace aiko
{
    namespace renderer
    {
        class RendererFactory;
    }

    class ScreenFbo
    {
    public:

        friend class renderer::RendererFactory;

        // Copy
        ScreenFbo(const ScreenFbo&) = delete;
        ScreenFbo& operator=(const ScreenFbo&) = delete;

        // Move
        ScreenFbo(ScreenFbo&&) noexcept = delete;
        ScreenFbo& operator=(ScreenFbo&&) noexcept = delete;

        ScreenFbo();
        ~ScreenFbo() = default;

        void use();
        void unuse();
        bool isValid();

        // load
        void create(int width, int height);
        void resize(int width, int height);
        void unload();

        // getters
        const FrameBuffer& getFrameBuffer() const { return m_frameBuffer; }
        const Mesh& getMesh() const { return m_mesh; }
        const Material& getMaterial() const { return m_material; }

    private:

        Shader passthrough;
        FrameBuffer m_frameBuffer;
        Mesh m_mesh;
        Material m_material;

    };

}
