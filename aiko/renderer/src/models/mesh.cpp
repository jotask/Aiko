#include "mesh.h"

#include "render_factory.h"

namespace aiko
{

    Mesh::Mesh()
        : backend(nullptr)
    {
        backend=renderer::RendererFactory::createMeshImpl(this);
    }

    void Mesh::refresh()
    {
        backend->refresh();
    }

    bool Mesh::isValid()
    {
        return backend->isValid();
    }

    void Mesh::unload()
    {
        backend->unload();
    }

}
