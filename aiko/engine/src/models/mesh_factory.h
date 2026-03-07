 #pragma once

#include <aiko_types.h>
#include <models/mesh.h>

 namespace aiko
{
    namespace mesh::factory
    {

        void recalculateNormals(MeshAsset& data);

        // 2D
        MeshAsset generatePoint();
        MeshAsset generateLine(vec3 start, vec3 end);
        MeshAsset generateQuad();
        MeshAsset generateCircle(uint segments = 32);
        MeshAsset generateTriangle();

        // 3D
        MeshAsset generatePyramid();
        MeshAsset generateMeshSphere(int rings, int slices);
        MeshAsset generateCube();
        MeshAsset generateMeshCylinder(int slices);
        MeshAsset generateMeshPlane(float width, float length, int resX, int resZ);
        MeshAsset generateMeshTorus();
        MeshAsset generateMeshKnot();

    }

}
