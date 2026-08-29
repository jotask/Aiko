#include "models/mesh.h"

#include "render_factory.h"

namespace aiko
{

    Mesh::Mesh(const MeshAsset& data)
        : Mesh()
    {
        upload(data);
    }

    Mesh::Mesh()
        : backend(nullptr)
    {
        backend = renderer::RendererFactory::createMeshImpl(this);
    }

    void Mesh::refresh()
    {

    }

    RenderResourceId Mesh::id() const
    {
        return backend->id();
    }

    void Mesh::upload(const MeshAsset& asset)
    {
        backend->refresh(asset);
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
