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
         Mesh(Mesh&&) noexcept = default;
         Mesh& operator=(Mesh&&) noexcept = default;

        Mesh(const MeshAsset& data);
        Mesh();
        ~Mesh() = default;

        void* getImpl() const { return backend.get(); }

        virtual bool isValid() const;
        virtual void unload();
        virtual void refresh();
        virtual uint id() const;

        void upload(const MeshAsset& asset);

     private:

        AikoPtr<interfaces::IMeshImpl> backend;

    };

}
