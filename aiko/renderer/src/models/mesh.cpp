#include "mesh.h"

#include "render_factory.h"

namespace aiko
{

    Mesh::Mesh(const MeshData data)
        : Mesh()
    {
        setData(data);
    }

    Mesh::Mesh()
        : backend(nullptr)
    {
        backend=renderer::RendererFactory::createMeshImpl(this);
    }

    void Mesh::refresh()
    {
        backend->refresh();
    }

    Mesh::MeshData Mesh::getData() const
    {
        return m_data;
    }

    void Mesh::setData(const MeshData data)
    {
        m_data = data;
        refresh();
    }

    bool Mesh::isValid() const
    {
        return backend->isValid();
    }

    void Mesh::unload()
    {
        backend->unload();
    }

}
