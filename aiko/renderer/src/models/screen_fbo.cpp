#include "screen_fbo.h"

#include "render_factory.h"

namespace aiko
{

    ScreenFbo::ScreenFbo()
    {

    }

    void ScreenFbo::use()
    {
    }

    void ScreenFbo::unuse()
    {
    }

    bool ScreenFbo::isValid()
    {
        return m_mesh.isValid() == true && m_material.m_shader.isValid() == true;
    }

    void ScreenFbo::create(int width, int height)
    {

        // Create mesh based on NDC
        {

            Mesh::MeshData data;

            {

                data.m_vertices = {
                        {-1.0f, -1.0f, 0.0f}, // bottom-left
                        {-1.0f,  1.0f, 0.0f}, // top-left
                        { 1.0f,  1.0f, 0.0f}, // top-right
                        { 1.0f, -1.0f, 0.0f}, // bottom-right
                    };

                data.m_textCoord = {
                        {0.0f, 0.0f},
                        {0.0f, 1.0f},
                        {1.0f, 1.0f},
                        {1.0f, 0.0f},
                    };

                data.m_normals = {
                        {0.0f, 0.0f, 1.0f},
                        {0.0f, 0.0f, 1.0f},
                        {0.0f, 0.0f, 1.0f},
                        {0.0f, 0.0f, 1.0f},
                    };

                data.m_colors = {
                    WHITE, WHITE, WHITE, WHITE
                };

                data.m_indices = {
                    0, 1, 2,
                    0, 2, 3
                };
            }

            m_mesh.setData(data);

        }

        // Set material
        {
            m_material.m_shader.load("passthrough");
            m_material.m_userVertexColor = false;
            m_material.m_lit = false;
            m_material.m_baseColor = WHITE;

            AIKO_ASSERT(m_material.m_shader.isValid(), "ScreenFbo shader invalid!");

        }

        resize(width, height);

    }

    void ScreenFbo::resize(int width, int height)
    {
        if (m_frameBuffer.isValid() == true)
        {
            m_frameBuffer.unload();
        }
        m_frameBuffer.create(width, height);
        AIKO_ASSERT(m_frameBuffer.isValid(), "ScreenFbo framebuffer invalid!");

        m_material.m_diffuse = m_frameBuffer.getColorTexture();

    }

    void ScreenFbo::unload()
    {
        m_mesh.unload();
        m_material.m_shader.unload();
    }

    FrameBuffer ScreenFbo::getFrameBuffer() const
    {
        return m_frameBuffer;
    }

    Mesh ScreenFbo::getMesh() const
    {
        return m_mesh;
    }

    Material ScreenFbo::getMaterial() const
    {
        return m_material;
    }

}
