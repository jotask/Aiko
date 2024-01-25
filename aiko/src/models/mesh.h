#pragma once

#include "aiko_types.h"
#include "camera_types.h"

class Mesh
{
public:
    
    enum Primitive {
        Circle,
        EmptyCircle,
        Cone,
        Cube,
        CubeIndexed,
        EmptyCube,
        Cylinder,
        Sphere,
        Square,
        EmptySquare,
        Screen,
    };

    Mesh();
    ~Mesh() = default;

    void init();

    void setPrimitive( Primitive );

private:


};