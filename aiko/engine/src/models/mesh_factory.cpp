#include "mesh_factory.h"

#include <math/math.h>

namespace aiko
{
    namespace mesh::factory
    {

        void recalculateNormals(MeshAsset& data)
        {

            const int vertexCount = static_cast<int>(data.m_vertices.size());

            if (vertexCount < 3)
            {
                data.m_normals.assign(data.m_vertices.size(), vec3(0.0f, 0.0f, 1.0f));
                return;
            }

            data.m_normals.assign(vertexCount, vec3(0.0f));

            for (size_t t = 0; t < data.m_indices.size(); t += 3)
            {
                const int i0 = data.m_indices[t + 0];
                const int i1 = data.m_indices[t + 1];
                const int i2 = data.m_indices[t + 2];

                const vec3& p0 = data.m_vertices[i0];
                const vec3& p1 = data.m_vertices[i1];
                const vec3& p2 = data.m_vertices[i2];

                const vec3 e1 = p1 - p0;
                const vec3 e2 = p2 - p0;

                vec3 n = math::normalize(math::cross(e2, e1));

                data.m_normals[i0] += n;
                data.m_normals[i1] += n;
                data.m_normals[i2] += n;
            }

            for (auto& n : data.m_normals)
            {
                n = math::normalize(n);
            }
        }

        // 3D

        MeshAsset generateCube()
        {

            MeshAsset data;

            data.m_vertices = {
                {-0.5f,  0.5f,  0.5f}, {-0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f,  0.5f}, { 0.5f,  0.5f,  0.5f}, // Front
                {-0.5f, -0.5f, -0.5f}, {-0.5f,  0.5f, -0.5f}, { 0.5f,  0.5f, -0.5f}, { 0.5f, -0.5f, -0.5f}, // Back
                { 0.5f,  0.5f,  0.5f}, { 0.5f,  0.5f, -0.5f}, {-0.5f,  0.5f, -0.5f}, {-0.5f,  0.5f,  0.5f}, // Top
                {-0.5f, -0.5f, -0.5f}, { 0.5f, -0.5f, -0.5f}, { 0.5f, -0.5f,  0.5f}, {-0.5f, -0.5f,  0.5f}, // Bottom
                { 0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f, -0.5f}, { 0.5f,  0.5f, -0.5f}, { 0.5f,  0.5f,  0.5f}, // Right
                {-0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f,  0.5f}, // Left
            };

            data.m_textCoord = vector<vec2>(data.m_vertices.size(), {0.0f,1.0f});

            data.m_colors = vector<Color>(data.m_vertices.size(), WHITE);

            data.m_indices = {
                0,2,1,      0,3,2,       // Front
                4,6,5,      4,7,6,       // Back
                8,10,9,     8,11,10,     // Top
                12,14,13,   12,15,14,    // Bottom
                16,18,17,   16,19,18,    // Right
                20,22,21,   20,23,22     // Left
            };
            recalculateNormals(data);
            return data;
        }

        MeshAsset generatePyramid()
        {

            MeshAsset data;

            const float h2 = 1.0f / 2.0f;
            const float b2 = 1.0f / 2.0f;

            data.m_vertices =
            {                              
                {-b2, -h2, -b2},
                { b2, -h2, -b2},
                { b2, -h2,  b2},
                {-b2, -h2,  b2},
                {0.0f, h2,  0.0f}
            };

            data.m_textCoord =
            {
                {0.0f, 1.0f},
                {1.0f, 1.0f},
                {1.0f, 0.0f},
                {0.0f, 0.0f},
                          
                {1.0f, 0.0f},
            };

            data.m_colors = vector<Color>(data.m_vertices.size(), WHITE);

            data.m_indices =
            {
                0, 2, 1, 2, 0, 3, // Bottom face
                0, 1, 4,           // Back-left triangle
                1, 2, 4,           // Back-right triangle
                2, 3, 4,           // Front-right triangle
                3, 0, 4            // Front-left triangle
            };
            recalculateNormals(data);
            return data;
        }

        MeshAsset generateMeshSphere(int rings, int slices)
        {

            MeshAsset data;

            constexpr const float r2 = 1.0f / 2.0f;

            auto vertices = data.m_vertices;
            auto indices = data.m_indices;

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

                    data.m_vertices.push_back({x, y, z});
                    data.m_textCoord.push_back({ u, v });
                    data.m_colors.push_back(WHITE);

                }
            }

            // Indices
            for (int i = 0; i < rings; ++i)
            {
                for (int j = 0; j < slices; ++j)
                {
                    int first = i * (slices + 1) + j;
                    int second = first + slices + 1;

                    data.m_indices.push_back(first);
                    data.m_indices.push_back(second);
                    data.m_indices.push_back(first + 1);

                    data.m_indices.push_back(second);
                    data.m_indices.push_back(second + 1);
                    data.m_indices.push_back(first + 1);
                }
            }
            recalculateNormals(data);
            return data;
        }

        MeshAsset generateMeshCylinder(int slices)
        {

            MeshAsset data;

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
                data.m_vertices.push_back({ x, -halfHeight, z });
                data.m_textCoord.push_back({ u, 0.0f });
                data.m_colors.push_back(WHITE);

                // Top circle vertex
                data.m_vertices.push_back({ x, halfHeight, z });
                data.m_textCoord.push_back({ u, 1.0f });
                data.m_colors.push_back(WHITE);

            }

            // Generate side indices
            for (int i = 0; i < slices; ++i)
            {
                int next = (i + 1) % slices;

                int bottom0 = i * 2;
                int top0 = i * 2 + 1;
                int bottom1 = next * 2;
                int top1 = next * 2 + 1;

                // First triangle (CW outward)
                data.m_indices.push_back(bottom0);
                data.m_indices.push_back(bottom1);
                data.m_indices.push_back(top0);

                // Second triangle (CW outward)
                data.m_indices.push_back(top0);
                data.m_indices.push_back(bottom1);
                data.m_indices.push_back(top1);
            }

            // Center vertices for caps
            const int bottomCenterIndex = int(data.m_vertices.size());
            data.m_vertices.push_back({ 0.0f, -halfHeight, 0.0f });
            data.m_textCoord.push_back({ 0.5f, 0.5f });
            data.m_colors.push_back(WHITE);

            // Generate bottom cap
            for (int i = 0; i < slices; ++i)
            {
                int bottom0 = i * 2;
                int bottom1 = (i + 1) * 2;
                data.m_indices.push_back(bottomCenterIndex);
                data.m_indices.push_back(bottom1);
                data.m_indices.push_back(bottom0);
            }

            const int topCenterIndex = bottomCenterIndex + 1;
            data.m_vertices.push_back({ 0.0f, halfHeight, 0.0f });
            data.m_textCoord.push_back({ 0.5f, 0.5f });
            data.m_colors.push_back(WHITE);

            // Generate top cap
            for (int i = 0; i < slices; ++i)
            {
                int top0 = i * 2 + 1;
                int top1 = ((i + 1) % slices) * 2 + 1; // wrap last slice
                data.m_indices.push_back(topCenterIndex);
                data.m_indices.push_back(top0);
                data.m_indices.push_back(top1);
            }
            recalculateNormals(data);
            return data;
        }

        MeshAsset generateMeshPlane(float width, float length, int resX, int resZ)
        {

            MeshAsset data;

            // Step size per grid cell
            float stepX = width / float(resX - 1);
            float stepZ = length / float(resZ - 1);

            // Generate vertices, UVs, colors
            for (int z = 0; z < resZ; ++z)
            {
                for (int x = 0; x < resX; ++x)
                {
                    float vx = -width / 2.0f + x * stepX;
                    float vz = -length / 2.0f + z * stepZ;
                    float vy = 0.0f; // flat plane on y=0

                    data.m_vertices.push_back({ vx, vy, vz });
                    data.m_textCoord.push_back({ float(x) / (resX - 1), float(z) / (resZ - 1) });
                    data.m_colors.push_back(WHITE); // all white
                }
            }

            // Generate indices (CW winding)
            for (int z = 0; z < resZ - 1; ++z)
            {
                for (int x = 0; x < resX - 1; ++x)
                {
                    int topLeft = z * resX + x;
                    int topRight = topLeft + 1;
                    int bottomLeft = (z + 1) * resX + x;
                    int bottomRight = bottomLeft + 1;

                    // First triangle
                    data.m_indices.push_back(topLeft);
                    data.m_indices.push_back(topRight);
                    data.m_indices.push_back(bottomLeft);

                    // Second triangle
                    data.m_indices.push_back(topRight);
                    data.m_indices.push_back(bottomRight);
                    data.m_indices.push_back(bottomLeft);
                }
            }

            // Generate normals (all pointing up)
            data.m_normals.resize(data.m_vertices.size(), { 0.0f, 1.0f, 0.0f });
            
            recalculateNormals(data);
            return data;
        }

        MeshAsset generateMeshTorus(float majorRadius, float minorRadius, uint radSeg, uint sides)
        {

            MeshAsset data;

            constexpr const Color color         = WHITE;

            const float maxDiameter = 2.0f * (majorRadius + minorRadius);
            const float scale = 1.0f / maxDiameter;

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

                    data.m_vertices.push_back(pos);
                    data.m_textCoord.push_back({ texU , texV });
                    data.m_colors.push_back(color);

                }
            }

            // Indices
            for (int i = 0; i < radSeg; ++i)
            {
                for (int j = 0; j < sides; ++j)
                {
                    int first = i * (sides + 1) + j;
                    int second = first + sides + 1;

                    data.m_indices.push_back(first);
                    data.m_indices.push_back(second);
                    data.m_indices.push_back(first + 1);

                    data.m_indices.push_back(second);
                    data.m_indices.push_back(second + 1);
                    data.m_indices.push_back(first + 1);
                }
            }
            recalculateNormals(data);
            return data;
        }

        MeshAsset generateMeshKnot(int p, int q, float radius, float tube, int radSeg, int sides)
        {

            MeshAsset data;

            constexpr const Color color = WHITE;

            constexpr const float size = 1.0f;

            // Store temporary vertices before scaling
            vector<vec3> positions;
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
            vec3 minV(FLOAT_MAX), maxV(-FLOAT_MAX);
            for (auto& p : positions)
            {
                minV.x = std::min(minV.x, p.x);
                minV.y = std::min(minV.y, p.y);
                minV.z = std::min(minV.z, p.z);

                maxV.x = std::max(maxV.x, p.x);
                maxV.y = std::max(maxV.y, p.y);
                maxV.z = std::max(maxV.z, p.z);
            }
            float maxExtent = std::max( maxV.x - minV.x, std::max( maxV.y - minV.y, maxV.z - minV.z ));
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

                    data.m_vertices.push_back(pos);
                    data.m_textCoord.push_back({u, v});
                    data.m_colors.push_back(color);
                }
            }

            // Indices
            for (int i = 0; i < radSeg; ++i)
            {
                for (int j = 0; j < sides; ++j)
                {
                    int first = i * (sides + 1) + j;
                    int second = first + sides + 1;

                    data.m_indices.push_back(first);
                    data.m_indices.push_back(second);
                    data.m_indices.push_back(first + 1);

                    data.m_indices.push_back(second);
                    data.m_indices.push_back(second + 1);
                    data.m_indices.push_back(first + 1);
                }
            }
            recalculateNormals(data);
            return data;
        }

        // 2D

        MeshAsset generatePoint()
        {
            MeshAsset data;
            data.m_vertices.push_back({ 0.0f });
            data.m_textCoord.push_back({ 0.0f }); // Not used
            data.m_colors.push_back(WHITE);
            data.m_indices.push_back(0);
            recalculateNormals(data);
            return data;
        }

        MeshAsset generateLine(vec3 start, vec3 end)
        {
            MeshAsset data;
            data.m_vertices.push_back(start);
            data.m_vertices.push_back(end);
            data.m_textCoord.push_back({ 0.0f, 1.0f });
            data.m_textCoord.push_back({ 0.0f, 1.0f });
            data.m_colors.push_back(WHITE);
            data.m_colors.push_back(WHITE);
            data.m_indices = { 0, 1 };
            recalculateNormals(data);
            return data;
        }
        
        MeshAsset generateQuad()
        {

            MeshAsset data;

            data.m_vertices =
            {
                { 0.5f,  0.5f, 0.0f},
                { 0.5f, -0.5f, 0.0f},
                {-0.5f, -0.5f, 0.0f},
                {-0.5f,  0.5f, 0.0f},
            };

            data.m_textCoord =
            {
                {1.0f, 1.0f},
                {1.0f, 0.0f},
                {0.0f, 0.0f},
                {0.0f, 1.0f},
            };

            data.m_colors = vector<Color>(data.m_vertices.size(), WHITE);

            data.m_indices =
            {
                0, 1, 3,
                1, 2, 3
            };
            recalculateNormals(data);
            return data;
        }

        MeshAsset generateCircle(uint segments)
        {

            MeshAsset data;

            constexpr float radius = 1.0f / 2.0f;
            constexpr Color color = WHITE;

            // Center vertex
            data.m_vertices.push_back({ 0.0f });
            data.m_textCoord.push_back({0.5f});
            data.m_colors.push_back(color);

            // Circle perimeter vertices
            for (int i = 0; i <= segments; ++i)
            {
                float angle = (2.0f * M_PI * i) / segments;
                float x = cosf(angle) * radius;
                float y = sinf(angle) * radius;

                data.m_vertices.push_back({x, y, 0.0f});
                data.m_textCoord.push_back({ 0.5f + 0.5f * cosf(angle), 0.5f + 0.5f * sinf(angle) });
                data.m_colors.push_back(color);

            }

            // Indices (triangle fan)
            data.m_indices.reserve(segments * 3);
            for (uint16_t i = 1; i <= segments; ++i)
            {
                data.m_indices.push_back(0);       // center
                data.m_indices.push_back(i + 1);   // next perimeter (wraps around)
                data.m_indices.push_back(i);         // current perimeter
            }
            recalculateNormals(data);
            return data;
        }

        MeshAsset generateTriangle()
        {

            MeshAsset data;

            data.m_vertices =
            {
                {-0.5f, -0.5f,  0.0f},
                { 0.5f, -0.5f,  0.0f},
                { 0.0f,  0.5f,  0.0f},
            };

            data.m_textCoord =
            {
                {0.0f, 1.0f},
                {0.0f, 0.0f},
                {1.0f, 0.0f},
            };

            data.m_colors = vector<Color>(data.m_vertices.size(), WHITE);

            data.m_indices =
            {
                0, 2, 1
            };

            recalculateNormals(data);
            return data;

        }

    }

}
