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
        void generateQuad(Mesh& mesh);
        void generateQuad(Mesh& mesh, Color color);
        void generateQuad(Mesh& mesh, VerticesColor colors);

        // 3D
        void generatePyramid(Mesh& mesh, float base, float height);
        void generateMeshSphere(Mesh& mesh, float radius, int rings, int slices);
        void generateCube(Mesh& mesh);
        void generateMeshCylinder(Mesh& mesh, float radius, float height, int slices);                         // Generate cylinder mesh

        void generateMeshPlane(Mesh& mesh, float width, float length, int resX, int resZ);                     // Generate plane mesh (with subdivisions)
        Mesh generateMeshPoly(int sides, float radius);                                            // Generate polygonal mesh
        void generateMeshCube(Mesh& mesh, float width, float height, float length);                            // Generate cuboid mesh
        Mesh generateMeshCone(float radius, float height, int slices);                             // Generate cone/pyramid mesh
        Mesh generateMeshTorus(float radius, float size, int radSeg, int sides);                   // Generate torus mesh
        Mesh generateMeshKnot(float radius, float size, int radSeg, int sides);                    // Generate trefoil knot mesh

    }

}
