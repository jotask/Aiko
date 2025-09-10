#include "mesh_factory.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace aiko
{
    namespace mesh
    {

        // CUBE

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

        // QUAD

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

        // Pyramids

        void generatePyramid(Mesh& mesh)
        {

            const float h2 = 1.0f / 2.0f;
            const float b2 = 1.0f / 2.0f;

            mesh.m_vertices =
            {

                  // Bottom face                                
                  -b2, -h2, -b2,      0.0f, 1.0f,         1.0f, 1.0f, 0.0f, // Back-bottom- left
                   b2, -h2, -b2,      1.0f, 1.0f,         1.0f, 1.0f, 0.0f, // Back-bottom-right
                   b2, -h2,  b2,      1.0f, 0.0f,         1.0f, 1.0f, 0.0f, // Front-bottom-right
                  -b2, -h2,  b2,      0.0f, 0.0f,         1.0f, 1.0f, 0.0f, // Front-bottom-left

                  // Top                                 
                   0.0f, h2,  0.0f,      1.0f, 0.0f,         1.0f, 0.0f, 1.0f, // Front-bottom-right

            };

            mesh.m_indices =
            {
                0, 1, 2, 2, 3, 0, // Bottom face
                0, 1, 4,           // Back-left triangle
                1, 2, 4,           // Back-right triangle
                2, 3, 4,           // Front-right triangle
                3, 0, 4            // Front-left triangle
            };
        }

        // Sphere

        void generateMeshSphere(Mesh& mesh, int rings, int slices)
        {

            constexpr const float r2 = 1.0f / 2.0f;

            auto vertices = mesh.m_vertices;
            auto indices = mesh.m_indices;

            // Vertices
            for (int i = 0; i <= rings; ++i)
            {
                float phi = float(i) / rings * float(M_PI); // 0 -> PI
                float y = r2 * cos(phi);
                float r = r2 * sin(phi); // radius of horizontal circle at this phi

                for (int j = 0; j <= slices; ++j)
                {
                    float theta = float(j) / slices * 2.0f * float(M_PI); // 0 -> 2PI
                    float x = r * cos(theta);
                    float z = r * sin(theta);

                    float u = float(j) / slices;
                    float v = float(i) / rings;

                    // Push vertex: x, y, z, u, v, r, g, b
                    mesh.m_vertices.push_back(x);
                    mesh.m_vertices.push_back(y);
                    mesh.m_vertices.push_back(z);

                    mesh.m_vertices.push_back(u);
                    mesh.m_vertices.push_back(v);

                    mesh.m_vertices.push_back(1.0f); // r
                    mesh.m_vertices.push_back(1.0f); // g
                    mesh.m_vertices.push_back(1.0f); // b
                }
            }

            // Indices
            for (int i = 0; i < rings; ++i)
            {
                for (int j = 0; j < slices; ++j)
                {
                    int first = i * (slices + 1) + j;
                    int second = first + slices + 1;

                    mesh.m_indices.push_back(first);
                    mesh.m_indices.push_back(second);
                    mesh.m_indices.push_back(first + 1);

                    mesh.m_indices.push_back(second);
                    mesh.m_indices.push_back(second + 1);
                    mesh.m_indices.push_back(first + 1);
                }
            }

        }

        // Cylinder

        void generateMeshCylinder(Mesh& mesh, int slices)
        {

            constexpr const float r2 = 1.0f / 2.0f;
            constexpr const float halfHeight = 1.0f / 2.0f;

            // Generate vertices
            for (int i = 0; i <= slices; ++i)
            {
                float theta = float(i) / slices * 2.0f * float(M_PI);
                float x = r2 * cos(theta);
                float z = r2 * sin(theta);

                float u = float(i) / slices;

                // Bottom circle vertex
                mesh.m_vertices.push_back(x);          // x
                mesh.m_vertices.push_back(-halfHeight); // y
                mesh.m_vertices.push_back(z);          // z
                mesh.m_vertices.push_back(u);          // u
                mesh.m_vertices.push_back(0.0f);      // v
                mesh.m_vertices.push_back(1.0f);      // r
                mesh.m_vertices.push_back(1.0f);      // g
                mesh.m_vertices.push_back(1.0f);      // b

                // Top circle vertex
                mesh.m_vertices.push_back(x);          // x
                mesh.m_vertices.push_back(halfHeight); // y
                mesh.m_vertices.push_back(z);          // z
                mesh.m_vertices.push_back(u);          // u
                mesh.m_vertices.push_back(1.0f);      // v
                mesh.m_vertices.push_back(1.0f);      // r
                mesh.m_vertices.push_back(1.0f);      // g
                mesh.m_vertices.push_back(1.0f);      // b
            }

            // Generate side indices
            for (int i = 0; i < slices; ++i)
            {
                int bottom0 = i * 2;
                int top0 = bottom0 + 1;
                int bottom1 = (i + 1) * 2;
                int top1 = bottom1 + 1;

                // Side quad (two triangles)
                mesh.m_indices.push_back(bottom0);
                mesh.m_indices.push_back(top0);
                mesh.m_indices.push_back(bottom1);

                mesh.m_indices.push_back(top0);
                mesh.m_indices.push_back(top1);
                mesh.m_indices.push_back(bottom1);
            }

            // Center vertices for caps
            int bottomCenterIndex = int(mesh.m_vertices.size() / 8); // 8 floats per vertex
            mesh.m_vertices.push_back(0.0f);             // x
            mesh.m_vertices.push_back(-halfHeight);      // y
            mesh.m_vertices.push_back(0.0f);             // z
            mesh.m_vertices.push_back(0.5f);             // u
            mesh.m_vertices.push_back(0.5f);             // v
            mesh.m_vertices.push_back(1.0f);             // r
            mesh.m_vertices.push_back(1.0f);             // g
            mesh.m_vertices.push_back(1.0f);             // b

            int topCenterIndex = bottomCenterIndex + 1;
            mesh.m_vertices.push_back(0.0f);             // x
            mesh.m_vertices.push_back(halfHeight);       // y
            mesh.m_vertices.push_back(0.0f);             // z
            mesh.m_vertices.push_back(0.5f);             // u
            mesh.m_vertices.push_back(0.5f);             // v
            mesh.m_vertices.push_back(1.0f);             // r
            mesh.m_vertices.push_back(1.0f);             // g
            mesh.m_vertices.push_back(1.0f);             // b

            // Generate bottom cap
            for (int i = 0; i < slices; ++i)
            {
                int bottom0 = i * 2;
                int bottom1 = (i + 1) * 2;
                mesh.m_indices.push_back(bottomCenterIndex);
                mesh.m_indices.push_back(bottom1);
                mesh.m_indices.push_back(bottom0);
            }

            // Generate top cap
            for (int i = 0; i < slices; ++i)
            {
                int top0 = i * 2 + 1;
                int top1 = (i + 1) * 2 + 1;
                mesh.m_indices.push_back(topCenterIndex);
                mesh.m_indices.push_back(top0);
                mesh.m_indices.push_back(top1);
            }
        }

        // Circle
        void generateCircle(Mesh& mesh, uint segments)
        {
            constexpr float radius = 1.0f / 2.0f;
            constexpr Color color = WHITE;

            mesh.m_vertices.reserve((segments + 1) * 8); // 8 floats per vertex (pos+uv+color)

            // Center vertex
            mesh.m_vertices.push_back(0.0f); // x
            mesh.m_vertices.push_back(0.0f); // y
            mesh.m_vertices.push_back(0.0f); // z
            mesh.m_vertices.push_back(0.5f); // u
            mesh.m_vertices.push_back(0.5f); // v
            mesh.m_vertices.push_back(color.r);
            mesh.m_vertices.push_back(color.g);
            mesh.m_vertices.push_back(color.b);

            // Circle perimeter vertices
            for (int i = 0; i <= segments; ++i)
            {
                float angle = (2.0f * M_PI * i) / segments;
                float x = cosf(angle) * radius;
                float y = sinf(angle) * radius;

                mesh.m_vertices.push_back(x);
                mesh.m_vertices.push_back(y);
                mesh.m_vertices.push_back(0.0f); // z
                mesh.m_vertices.push_back(0.5f + 0.5f * cosf(angle)); // u
                mesh.m_vertices.push_back(0.5f + 0.5f * sinf(angle)); // v
                mesh.m_vertices.push_back(color.r);
                mesh.m_vertices.push_back(color.g);
                mesh.m_vertices.push_back(color.b);
            }

            // Indices (triangle fan)
            mesh.m_indices.reserve(segments * 3);
            for (uint16_t i = 1; i <= segments; ++i)
            {
                mesh.m_indices.push_back(0);       // center
                mesh.m_indices.push_back(i);       // current perimeter
                mesh.m_indices.push_back(i + 1);   // next perimeter (wraps around)
            }
        }

        // Triangle
        void generateTriangle(Mesh& mesh)
        {
            mesh.m_vertices =
            {
                -0.5f, -0.5f,  0.0f,       0.0f, 1.0f,        1.0f, 1.0f, 1.0f, // Top-left
                 0.5f, -0.5f,  0.0f,       0.0f, 0.0f,        1.0f, 1.0f, 1.0f, // Bottom-left
                 0.0f,  0.5f,  0.0f,       1.0f, 0.0f,        1.0f, 1.0f, 1.0f, // Bottom-right
            };

            mesh.m_indices =
            {
                0, 1, 2
            };
        }

        // Others

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

        void generateMeshTorus(Mesh& mesh)
        {

            constexpr const float majorRadius   = 1.0f;
            constexpr const float minorRadius   = 0.3f;
            constexpr const uint radSeg         = 32;
            constexpr const uint sides          = 16;
            constexpr const Color color         = WHITE;

            constexpr const float maxDiameter = 2.0f * (majorRadius + minorRadius);
            constexpr const float scale = 1.0f / maxDiameter;

            for (int i = 0; i <= radSeg; ++i)
            {
                float u = (float)i / radSeg * 2.0f * M_PI;
                float cosU = cosf(u);
                float sinU = sinf(u);

                for (int j = 0; j <= sides; ++j)
                {
                    float v = (float)j / sides * 2.0f * M_PI;
                    float cosV = cosf(v);
                    float sinV = sinf(v);

                    vec3 pos;
                    pos.x = (majorRadius + minorRadius * cosV) * cosU;
                    pos.y = minorRadius * sinV;
                    pos.z = (majorRadius + minorRadius * cosV) * sinU;

                    pos *= scale;

                    float texU = (float)i / radSeg;
                    float texV = (float)j / sides;

                    mesh.m_vertices.push_back(pos.x);
                    mesh.m_vertices.push_back(pos.y);
                    mesh.m_vertices.push_back(pos.z);
                    mesh.m_vertices.push_back(texU);
                    mesh.m_vertices.push_back(texV);
                    mesh.m_vertices.push_back(color.r);
                    mesh.m_vertices.push_back(color.g);
                    mesh.m_vertices.push_back(color.b);
                }
            }

            // Indices
            for (int i = 0; i < radSeg; ++i)
            {
                for (int j = 0; j < sides; ++j)
                {
                    int first = i * (sides + 1) + j;
                    int second = first + sides + 1;

                    mesh.m_indices.push_back(first);
                    mesh.m_indices.push_back(second);
                    mesh.m_indices.push_back(first + 1);

                    mesh.m_indices.push_back(second);
                    mesh.m_indices.push_back(second + 1);
                    mesh.m_indices.push_back(first + 1);
                }
            }
        }

        void generateMeshKnot(Mesh& mesh)
        {

            constexpr const int p = 2;
            constexpr const int q = 3;
            constexpr const float radius = 1.0f;
            constexpr const float tube = 0.2f;
            constexpr const int radSeg = 128;
            constexpr const int sides = 16;
            constexpr const Color color = WHITE;

            constexpr const float size = 1.0f;

            // Store temporary vertices before scaling
            std::vector<vec3> positions;
            positions.reserve((radSeg + 1) * (sides + 1));

            for (int i = 0; i <= radSeg; ++i)
            {
                float t = (float)i / radSeg * 2.0f * M_PI;

                // Torus knot center point
                float x = (radius + tube * cosf(q * t)) * cosf(p * t);
                float y = (radius + tube * cosf(q * t)) * sinf(p * t);
                float z = tube * sinf(q * t);

                // Tangent along the knot
                float dx = -p * (radius + tube * cosf(q * t)) * sinf(p * t) - tube * q * sinf(q * t) * cosf(p * t);
                float dy = p * (radius + tube * cosf(q * t)) * cosf(p * t) - tube * q * sinf(q * t) * sinf(p * t);
                float dz = tube * q * cosf(q * t);
                vec3 tangent = math::normalize(vec3(dx, dy, dz));

                // Perpendicular frame
                vec3 normal = math::normalize(math::cross(tangent, vec3(0, 0, 1)));
                vec3 binormal = math::cross(tangent, normal);

                for (int j = 0; j <= sides; ++j)
                {
                    float phi = (float)j / sides * 2.0f * M_PI;
                    float cx = cosf(phi) * tube;
                    float cy = sinf(phi) * tube;

                    vec3 pos = vec3(x, y, z) + normal * cx + binormal * cy;
                    positions.push_back(pos);
                }
            }

            // Find bounding box and compute scale
            vec3 minV(FLT_MAX), maxV(-FLT_MAX);
            for (auto& p : positions)
            {
                minV.x = std::min(minV.x, p.x);
                minV.y = std::min(minV.y, p.y);
                minV.z = std::min(minV.z, p.z);

                maxV.x = std::max(maxV.x, p.x);
                maxV.y = std::max(maxV.y, p.y);
                maxV.z = std::max(maxV.z, p.z);
            }
            float maxExtent = std::max({ maxV.x - minV.x, maxV.y - minV.y, maxV.z - minV.z });
            float scale = size / maxExtent;

            // Write final vertices into mesh
            int idx = 0;
            for (int i = 0; i <= radSeg; ++i)
            {
                for (int j = 0; j <= sides; ++j)
                {
                    vec3 pos = positions[idx++] * scale;

                    float u = (float)i / radSeg;
                    float v = (float)j / sides;

                    mesh.m_vertices.push_back(pos.x);
                    mesh.m_vertices.push_back(pos.y);
                    mesh.m_vertices.push_back(pos.z);
                    mesh.m_vertices.push_back(u);
                    mesh.m_vertices.push_back(v);
                    mesh.m_vertices.push_back(color.r);
                    mesh.m_vertices.push_back(color.g);
                    mesh.m_vertices.push_back(color.b);
                }
            }

            // Indices
            for (int i = 0; i < radSeg; ++i)
            {
                for (int j = 0; j < sides; ++j)
                {
                    int first = i * (sides + 1) + j;
                    int second = first + sides + 1;

                    mesh.m_indices.push_back(first);
                    mesh.m_indices.push_back(second);
                    mesh.m_indices.push_back(first + 1);

                    mesh.m_indices.push_back(second);
                    mesh.m_indices.push_back(second + 1);
                    mesh.m_indices.push_back(first + 1);
                }
            }
        }

        // 2D

        void generatePoint(Mesh& mesh)
        {
            constexpr const Color color = WHITE;

            mesh.m_vertices.clear();
            mesh.m_indices.clear();

            // Add vertex: x, y, z, u, v, r, g, b
            mesh.m_vertices.push_back(0.0f);
            mesh.m_vertices.push_back(0.0f);
            mesh.m_vertices.push_back(0.0f);

            mesh.m_vertices.push_back(0.0f); // u (not used)
            mesh.m_vertices.push_back(0.0f); // v (not used)

            mesh.m_vertices.push_back(color.r);
            mesh.m_vertices.push_back(color.g);
            mesh.m_vertices.push_back(color.b);

            // Add a single index
            mesh.m_indices.push_back(0);

        }

    }

}
