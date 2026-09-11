#pragma once

#include "models/mesh.h"

namespace aiko
{
    class ScreenPresenter
    {
    public:
        void init();
        void dispose();

        bool isValid() const;

        const Mesh& mesh() const
        {
            return m_mesh;
        }

    private:
        Mesh m_mesh;
    };
}
