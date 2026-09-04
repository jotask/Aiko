#include "screen_fbo.h"

#include "render_factory.h"

namespace aiko
{

    ScreenFbo::ScreenFbo()
    {

    }

    bool ScreenFbo::isValid()
    {
        return m_mesh.isValid() == true && m_material.m_shaderId != InvalidAssetId;
    }

    void ScreenFbo::create(int width, int height)
    {

        // Create mesh based on NDC
        {

            MeshAsset data;

            {

                data.m_vertices = {
                        {-1.0f, -1.0f, 0.0f}, // bottom-left
                        {-1.0f,  1.0f, 0.0f}, // top-left
                        { 1.0f,  1.0f, 0.0f}, // top-right
                        { 1.0f, -1.0f, 0.0f}, // bottom-right
                    };

                data.m_textCoord = {
                        {0.0f, 1.0f},
                        {0.0f, 0.0f},
                        {1.0f, 0.0f},
                        {1.0f, 1.0f},
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

            m_mesh.upload(data);

        }
        resize(width, height);

    }

    void ScreenFbo::resize(int width, int height)
    {
        m_frameBuffer.create(width, height);
        AIKO_ASSERT(m_frameBuffer.isValid(), "ScreenFbo framebuffer invalid!");

        m_material.m_runtimeDiffuseTexture = &m_frameBuffer.getColorTexture();
        m_material.m_diffuseTextureId = InvalidAssetId;

    }

    void ScreenFbo::unload()
    {
        m_mesh.unload();
        m_frameBuffer.unload();
        m_material.m_runtimeDiffuseTexture = nullptr;
    }

    void ScreenFbo::setMaterial(Material&& material)
    {
        m_material = std::move(material);
    }

}
