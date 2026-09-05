#include "screen_presenter.h"

#include "assets/types/mesh_asset.h"
#include "types/color.h"

namespace aiko
{
    void ScreenPresenter::init()
    {
        if (m_mesh.isValid())
        {
            m_mesh.unload();
        }

        MeshAsset data;

        data.m_vertices =
        {
                {-1.0f, -1.0f, 0.0f},
                {-1.0f,  1.0f, 0.0f},
                { 1.0f,  1.0f, 0.0f},
                { 1.0f, -1.0f, 0.0f},
            };

        data.m_textCoord =
        {
                {0.0f, 1.0f},
                {0.0f, 0.0f},
                {1.0f, 0.0f},
                {1.0f, 1.0f},
            };

        data.m_normals =
        {
                {0.0f, 0.0f, 1.0f},
                {0.0f, 0.0f, 1.0f},
                {0.0f, 0.0f, 1.0f},
                {0.0f, 0.0f, 1.0f},
            };

        data.m_colors =
        {
            WHITE,
            WHITE,
            WHITE,
            WHITE
        };

        data.m_indices =
        {
            0, 1, 2,
            0, 2, 3
        };

        m_mesh.upload(data);
    }

    void ScreenPresenter::dispose()
    {
        m_mesh.unload();
    }

    bool ScreenPresenter::isValid() const
    {
        return m_mesh.isValid();
    }
}
