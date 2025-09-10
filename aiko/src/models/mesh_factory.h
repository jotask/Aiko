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

        // 2D
        void generatePoint(Mesh& mesh);
        void generateCircle(Mesh& mesh, uint segments = 32);
        void generateTriangle(Mesh& mesh);
        void generateQuad(Mesh& mesh);
        void generateQuad(Mesh& mesh, Color color);
        void generateQuad(Mesh& mesh, VerticesColor colors);

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
