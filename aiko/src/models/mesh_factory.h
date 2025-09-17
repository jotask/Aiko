 #pragma once

#include <vector>
#include <array>

#include "aiko_types.h"
#include "models/mesh.h"
#include "types/color.h"

namespace aiko
{
    namespace mesh
    {

        using VerticesColor = std::vector<aiko::Color>;

        void generateNormals(Mesh& mesh);

        // 2D
        void generatePoint(Mesh& mesh);
        void generateLine(Mesh& mesh, vec3 start, vec3 end);
        void generateQuad(Mesh& mesh);
        void generateCircle(Mesh& mesh, uint segments = 32);
        void generateTriangle(Mesh& mesh);

        // 3D
        void generatePyramid(Mesh& mesh);
        void generateMeshSphere(Mesh& mesh, int rings, int slices);
        void generateCube(Mesh& mesh);
        void generateMeshCylinder(Mesh& mesh, int slices);
        void generateMeshPlane(Mesh& mesh, float width, float length, int resX, int resZ);
        void generateMeshTorus(Mesh& mesh);
        void generateMeshKnot(Mesh& mesh);

    }

}
