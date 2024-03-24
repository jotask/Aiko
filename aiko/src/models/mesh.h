 #pragma once

#include <vector>

#include "aiko_types.h"
#include "types/camera_types.h"
#include "types/color.h"
#include "shared/math.h"
#include "models/shader.h"
#include "models/shader.h"

namespace aiko
{
    
    class Shader;
    class Mesh
    {
    public:

        friend class RenderModule;
        friend class RenderSystem;
    
        Mesh();
        ~Mesh() = default;

    //private:

        using Vertices = std::vector<float>;
        using TeexCoords = std::vector<float>;
        using TeexCoords2 = std::vector<float>;
        using Normals = std::vector<float>;
        using Colors = std::vector<unsigned char>;
        using Indices = std::vector<unsigned short>;

        Vertices m_vertices;
        TeexCoords m_teexCoord;
        TeexCoords2 m_teexCoord2;
        Normals m_normals;
        Colors m_colors;
        Indices m_indices;

        MeshData m_data;
        Shader* m_shader;

    };

}
