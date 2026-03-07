 #pragma once

#include <aiko_types.h>
#include <math/math.h>

#include "texture.h"
#include "frame_buffer.h"
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
        ScreenFbo(const ScreenFbo&) = default;
        ScreenFbo& operator=(const ScreenFbo&) = default;

        // Move
        ScreenFbo(ScreenFbo&&) noexcept = default;
        ScreenFbo& operator=(ScreenFbo&&) noexcept = default;

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
        FrameBuffer getFrameBuffer() const;
        Mesh getMesh() const;
        Material getMaterial() const;

    private:

        FrameBuffer m_frameBuffer;
        Mesh m_mesh;
        Material m_material;

    };

}
