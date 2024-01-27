#include "mesh.h"

#include <stdexcept>

namespace aiko
{
    
    Mesh::Mesh()
    {
    
    }
    
    void Mesh::init()
    {
    
    }
    
    void Mesh::setPrimitive(Primitive primitive)
    {
        constexpr auto SIZE = 256;
        switch (primitive)
        {
        case Primitive::Sphere:
    
            break;
        case Primitive::Cone:
    
            break;
        case Primitive::Cylinder:
    
            break;
        default:
            throw std::exception();
            break;
        }
    }

}
