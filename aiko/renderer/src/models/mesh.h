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

        friend class renderer::RendererFactory;

         // Copy
         Mesh(const Mesh&) = default;
         Mesh& operator=(const Mesh&) = default;

         // Move
         Mesh(Mesh&&) noexcept = default;
         Mesh& operator=(Mesh&&) noexcept = default;

        Mesh();
        ~Mesh() = default;

         virtual bool isValid();
         virtual void unload();
         virtual void refresh();

    // private:

        using Vertices = std::vector<vec3>;
        using TeexCoords = std::vector<vec2>;
        using Normals = std::vector<vec3>;
        using Colors = std::vector<Color>;
        using Indices = std::vector<uint32_t>;

        Vertices m_vertices;
        TeexCoords m_teexCoord;
        Normals m_normals;
        Colors m_colors;
        Indices m_indices;

     private:

        AikoPtr<interfaces::IMeshImpl> backend;

    };

}
