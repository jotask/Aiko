 #pragma once

#include <aiko_types.h>
#include <models/mesh.h>

namespace aiko
{
    namespace mesh::factory
    {

        void recalculateNormals(Mesh::MeshData& data);

        // 2D
        Mesh::MeshData generatePoint();
        Mesh::MeshData generateLine(vec3 start, vec3 end);
        Mesh::MeshData generateQuad();
        Mesh::MeshData generateCircle(uint segments = 32);
        Mesh::MeshData generateTriangle();

        // 3D
        Mesh::MeshData generatePyramid();
        Mesh::MeshData generateMeshSphere(int rings, int slices);
        Mesh::MeshData generateCube();
        Mesh::MeshData generateMeshCylinder();
        Mesh::MeshData generateMeshPlane(float width, float length, int resX, int resZ);
        Mesh::MeshData generateMeshTorus();
        Mesh::MeshData generateMeshKnot();

    }

}
