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
        MeshAsset generateCircle(uint segments);
        MeshAsset generateTriangle();

        // 3D
        MeshAsset generatePyramid();
        MeshAsset generateMeshSphere(int rings, int slices);
        MeshAsset generateCube();
        MeshAsset generateMeshCylinder(int slices);
        MeshAsset generateMeshPlane(float width, float length, int resX, int resZ);
        MeshAsset generateMeshTorus(float majorRadius = 1.0f, float minorRadius = 0.3f, uint radSeg = 32, uint sides = 16);
        MeshAsset generateMeshKnot(int p = 2, int q = 3, float radius = 1.0f, float tube = 0.2f, int radSeg = 128, int sides = 16);

    }

}
