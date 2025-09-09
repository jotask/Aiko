#include "mesh_factory.h"

namespace aiko
{
    namespace mesh
    {
        void generateCube(Mesh& mesh)
        {

            mesh.m_vertices =
            {
                // positions              // texture coords   // Color
                // Front face
                -0.5f,  0.5f,  0.5f,       0.0f, 1.0f,        1.0f, 0.0f, 0.0f, // Top-left
                -0.5f, -0.5f,  0.5f,       0.0f, 0.0f,        1.0f, 0.0f, 0.0f, // Bottom-left
                 0.5f, -0.5f,  0.5f,       1.0f, 0.0f,        1.0f, 0.0f, 0.0f, // Bottom-right
                 0.5f,  0.5f,  0.5f,       1.0f, 1.0f,        1.0f, 0.0f, 0.0f, // Top-right

                 // Back face                                  
                 -0.5f, -0.5f, -0.5f,       0.0f, 0.0f,         0.0f, 1.0f, 0.0f, // Bottom-left
                 -0.5f,  0.5f, -0.5f,       0.0f, 1.0f,         0.0f, 1.0f, 0.0f, // Top-left
                  0.5f,  0.5f, -0.5f,       1.0f, 1.0f,         0.0f, 1.0f, 0.0f, // Top-right
                  0.5f, -0.5f, -0.5f,       1.0f, 0.0f,         0.0f, 1.0f, 0.0f, // Bottom-right

                  // Top face                                   
                   0.5f,  0.5f,  0.5f,       1.0f, 0.0f,         0.0f, 0.0f, 1.0f, // Front-top-right
                   0.5f,  0.5f, -0.5f,       1.0f, 1.0f,         0.0f, 0.0f, 1.0f, // Back-top-right
                  -0.5f,  0.5f, -0.5f,      0.0f, 1.0f,         0.0f, 0.0f, 1.0f, // Back-top-left
                  -0.5f,  0.5f,  0.5f,      0.0f, 0.0f,         0.0f, 0.0f, 1.0f, // Front-top-left

                  // Bottom face                                
                  -0.5f, -0.5f, -0.5f,      0.0f, 1.0f,         1.0f, 1.0f, 0.0f, // Back-bottom- left
                   0.5f, -0.5f, -0.5f,       1.0f, 1.0f,         1.0f, 1.0f, 0.0f, // Back-bottom-right
                   0.5f, -0.5f,  0.5f,       1.0f, 0.0f,         1.0f, 1.0f, 0.0f, // Front-bottom-right
                  -0.5f, -0.5f,  0.5f,      0.0f, 0.0f,         1.0f, 1.0f, 0.0f, // Front-bottom-left

                  // Right face                                 
                   0.5f, -0.5f,  0.5f,      1.0f, 0.0f,         1.0f, 0.0f, 1.0f, // Front-bottom-right
                   0.5f, -0.5f, -0.5f,      1.0f, 1.0f,         1.0f, 0.0f, 1.0f, // Back-bottom-right
                   0.5f,  0.5f, -0.5f,      0.0f, 1.0f,         1.0f, 0.0f, 1.0f, // Back-top-right
                   0.5f,  0.5f,  0.5f,      0.0f, 0.0f,         1.0f, 0.0f, 1.0f, // Front-top-right

                   // Left face                                  
                   -0.5f,  0.5f,  0.5f,      0.0f, 1.0f,         0.0f, 1.0f, 1.0f, // Front-top-left
                   -0.5f,  0.5f, -0.5f,      1.0f, 1.0f,         0.0f, 1.0f, 1.0f, // Back-top-left
                   -0.5f, -0.5f, -0.5f,      1.0f, 0.0f,         0.0f, 1.0f, 1.0f, // Back-bottom-left
                   -0.5f, -0.5f,  0.5f,      0.0f, 0.0f,         0.0f, 1.0f, 1.0f, // Front-bottom-left
            };

            mesh.m_indices =
            {
                0, 1, 2, 2, 3, 0,       // Front face
                4, 5, 6, 6, 7, 4,       // Back face
                8, 9, 10, 10, 11, 8,    // Top face
                12, 13, 14, 14, 15, 12, // Bottom face
                16, 17, 18, 18, 19, 16, // Right face
                20, 21, 22, 22, 23, 20  // Left face
            };

        }

        void generateQuad(Mesh& mesh)
        {
            VerticesColor colors =
            {
                aiko::Color( 1.0f, 0.0f, 0.0f, 1.0f ),
                aiko::Color( 0.0f, 1.0f, 0.0f, 1.0f ),
                aiko::Color( 0.0f, 0.0f, 1.0f, 1.0f ),
                aiko::Color( 1.0f, 1.0f, 0.0f, 1.0f),
            };
            return generateQuad(mesh, colors);
        }

        void generateQuad(Mesh& mesh, aiko::Color color)
        {
            VerticesColor colors =
            {
                color,
                color,
                color,
                color,
            };
            return generateQuad(mesh, colors);
        }

        void generateQuad(Mesh& mesh, VerticesColor c)
        {
            mesh.m_vertices =
            {
                  // positions           // texture coords      // colors         
                   0.5f,  0.5f, 0.0f,     1.0f, 1.0f,            c[0].r, c[0].g, c[0].b, // top right
                   0.5f, -0.5f, 0.0f,     1.0f, 0.0f,            c[1].r, c[1].g, c[1].b, // bottom right
                  -0.5f, -0.5f, 0.0f,     0.0f, 0.0f,            c[2].r, c[2].g, c[2].b, // bottom left
                  -0.5f,  0.5f, 0.0f,     0.0f, 1.0f,            c[3].r, c[3].g, c[3].b, // top left
            };

            mesh.m_indices =
            {
                0, 1, 3, // first triangle
                1, 2, 3  // second triangle
            };
        }

        void generateMeshPlane(Mesh& mesh, float width, float length, int resX, int resZ)
        {

            resX++;
            resZ++;

            // Vertices definition
            int vertexCount = resX * resZ; // vertices get reused for the faces

            mesh.m_vertices.resize(vertexCount * sizeof(vec3));
            for (int z = 0; z < resZ; z++)
            {
                // [-length/2, length/2]
                float zPos = ((float)z / (resZ - 1) - 0.5f) * length;
                for (int x = 0; x < resX; x++)
                {
                    // [-width/2, width/2]
                    float xPos = ((float)x / (resX - 1) - 0.5f) * width;
                    mesh.m_vertices[x + z * resX + 0] = xPos;
                    mesh.m_vertices[x + z * resX + 1] = 0.0f;
                    mesh.m_vertices[x + z * resX + 2] = zPos;
                }
            }

            // Normals definition
            mesh.m_normals.resize(vertexCount * sizeof(vec3));
            for (int n = 0; n < vertexCount; n++)
            {
                mesh.m_normals[n + 0] = 0.0f;   // vec3.up;
                mesh.m_normals[n + 1] = 1.0f;   // vec3.up;
                mesh.m_normals[n + 2] = 0.0f;   // vec3.up;
            }

            // TexCoords definition
            mesh.m_teexCoord.resize(vertexCount * sizeof(vec2));
            for (int v = 0; v < resZ; v++)
            {
                for (int u = 0; u < resX; u++)
                {
                    mesh.m_teexCoord[u + v * resX + 0] = (float)u / (resX - 1);
                    mesh.m_teexCoord[u + v * resX + 1] = (float)v / (resZ - 1);
                }
            }

            // Triangles definition (indices)
            int numFaces = (resX - 1) * (resZ - 1);
            mesh.m_indices.resize(numFaces * 6 * sizeof(int));
            int t = 0;
            for (int face = 0; face < numFaces; face++)
            {
                // Retrieve lower left corner from face ind
                int i = face + face / (resX - 1);

                mesh.m_indices[t++] = i + resX;
                mesh.m_indices[t++] = i + 1;
                mesh.m_indices[t++] = i;

                mesh.m_indices[t++] = i + resX;
                mesh.m_indices[t++] = i + resX + 1;
                mesh.m_indices[t++] = i + 1;
            }

        }

        /*
        Mesh generateMeshPoly(int sides, float radius)
        {
            ::Mesh m = ::GenMeshPoly(sides, radius);
            return raylib::utils::toMesh( m );
        }

        Mesh generateMeshSphere(float radius, int rings, int slices)
        {
            ::Mesh m = ::GenMeshSphere( radius, rings, slices);
            return raylib::utils::toMesh(m);
        }

        Mesh generateMeshCylinder(float radius, float height, int slices)
        {
            ::Mesh m = ::GenMeshCylinder( radius, height, slices);
            return raylib::utils::toMesh(m);
        }

        Mesh generateMeshCone(float radius, float height, int slices)
        {
            ::Mesh m = ::GenMeshCone( radius, height, slices);
            return raylib::utils::toMesh(m);
        }

        Mesh generateMeshTorus(float radius, float size, int radSeg, int sides)
        {
            ::Mesh m = ::GenMeshTorus( radius, size, radSeg, sides);
            return raylib::utils::toMesh(m);
        }

        Mesh generateMeshKnot(float radius, float size, int radSeg, int sides)
        {
            ::Mesh m = ::GenMeshKnot( radius, size, radSeg, sides);
            return raylib::utils::toMesh(m);
        }
        */
    }

}
