#include "models/mesh.h"

#include "render_factory.h"
#include "assets/types/mesh_asset.h"
#include "interfaces/imesh_impl.h"

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

    Mesh::~Mesh()
    {
        if (backend != nullptr)
        {
            backend->unload();
        }
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
