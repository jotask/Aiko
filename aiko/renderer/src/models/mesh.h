 #pragma once

#include <vector>

#include <math/math.h>

#include "aiko_types.h"
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

         struct MeshData
         {

             using Vertices = std::vector<vec3>;
             using TeexCoords = std::vector<vec2>;
             using Normals = std::vector<vec3>;
             using Colors = std::vector<Color>;
             using Indices = std::vector<uint32_t>;

             Vertices m_vertices;
             TeexCoords m_textCoord;
             Normals m_normals;
             Colors m_colors;
             Indices m_indices;
         };

        friend class renderer::RendererFactory;

         // Copy
         Mesh(const Mesh&) = default;
         Mesh& operator=(const Mesh&) = default;

         // Move
         Mesh(Mesh&&) noexcept = default;
         Mesh& operator=(Mesh&&) noexcept = default;

        Mesh(const MeshData data);
        Mesh();
        ~Mesh() = default;

        void* getImpl() const { return backend.get(); }

        virtual bool isValid() const;
        virtual void unload();
        virtual void refresh();
        virtual uint id() const;

        void load(string file);

        MeshData getData() const;
        void setData(const MeshData data);

     private:

        AikoPtr<interfaces::IMeshImpl> backend;
        MeshData m_data;

    };

}
