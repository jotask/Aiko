 #pragma once

#include <vector>

#include "aiko_types.h"
#include "types/camera_types.h"
#include "types/color.h"
#include "math/math.h"
#include "time/shader.h"
#include "time/texture.h"

 namespace aiko
{
    class Mesh
    {
    public:

        enum class MeshType
        {
            CUBE,
            QUAD,
            CUSTOM,
        };

        friend class RenderModule;
        friend class RenderSystem;
    
        Mesh();
        ~Mesh() = default;

        void refresh();

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

        MeshData m_data;
        Shader* m_shader;

    };

}
