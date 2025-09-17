#include "mesh.h"

#include <stdexcept>

#include "modules/render/render_module.h"

namespace aiko
{

    RenderModule* Mesh::s_renderModule = nullptr;

    Mesh::Mesh()
    {
        s_renderModule->initMesh(this);
    }

    void Mesh::refresh()
    {
        s_renderModule->refreshMesh(this);
    }

}
