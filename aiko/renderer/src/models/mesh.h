 #pragma once

#include "aiko_types.h"
#include "assets/types/mesh_asset.h"
#include "types/color.h"
#include "interfaces/imesh_impl.h"

 namespace aiko
{

     namespace renderer
     {
         class RendererFactory;
     }

    class Mesh
    {
    public:

        friend class renderer::RendererFactory;

         // Copy
         Mesh(const Mesh&) = delete;
         Mesh& operator=(const Mesh&) = delete;

         // Move
         Mesh(Mesh&&) noexcept = delete;
         Mesh& operator=(Mesh&&) noexcept = delete;

        Mesh(const MeshAsset& data);
        Mesh();
        ~Mesh();

        void* getImpl() const { return backend.get(); }

        virtual bool isValid() const;
        virtual void unload();
        virtual void refresh();
        virtual RenderResourceId id() const;

        void upload(const MeshAsset& asset);

     private:

        AikoPtr<interfaces::IMeshImpl> backend;

    };

}
