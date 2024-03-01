 #pragma once

#include <vector>

#include "aiko_types.h"
#include "models/mesh.h"

namespace aiko
{
    namespace mesh
    {

        // And again thanks raylib
        // I'm not trying to copy raysan work! I just want a working enviorement, where i can play around, then dedicate time to start removing libraries
        // All I can say is to say thanks to raysan!

        Mesh generateMeshPoly(int sides, float radius);                                            // Generate polygonal mesh
        Mesh generateMeshPlane(float width, float length, int resX, int resZ);                     // Generate plane mesh (with subdivisions)
        Mesh generateMeshCube(float width, float height, float length);                            // Generate cuboid mesh
        Mesh generateMeshSphere(float radius, int rings, int slices);                              // Generate sphere mesh (standard sphere)
        Mesh generateMeshCylinder(float radius, float height, int slices);                         // Generate cylinder mesh
        Mesh generateMeshCone(float radius, float height, int slices);                             // Generate cone/pyramid mesh
        Mesh generateMeshTorus(float radius, float size, int radSeg, int sides);                   // Generate torus mesh
        Mesh generateMeshKnot(float radius, float size, int radSeg, int sides);                    // Generate trefoil knot mesh

    }

}
