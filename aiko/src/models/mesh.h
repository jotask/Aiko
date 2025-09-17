 #pragma once

#include <vector>

#include "aiko_types.h"
#include "types/camera_types.h"
#include "types/color.h"
#include "shared/math.h"
#include "models/shader.h"
#include "models/texture.h"

namespace aiko
{
    class RenderModule;
    class Mesh
    {
    private:
        friend class RenderModule;
        static RenderModule* s_renderModule;
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
