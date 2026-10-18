#include "mesh_tangents.h"

#include <math/math.h>

#include <cmath>

namespace aiko::mesh
{
    vector<vec4> calculateTangents(const MeshAsset& asset)
    {
        const size_t count = asset.m_vertices.size();

        vector<vec4> result(count, vec4{0.0f, 0.0f, 0.0f, 1.0f});

        // Tangents require complete UVs, normals and triangles.
        if (count == 0 || asset.m_textCoord.size() != count || asset.m_normals.size() != count || asset.m_indices.empty() || asset.m_indices.size() % 3 != 0)
        {
            return result;
        }

        vector<vec3> tangentSum(count, vec3{0.0f});
        vector<vec3> bitangentSum(count, vec3{0.0f});

        // 0 = unset, +/-1 = handedness, 2 = conflicting UV orientations.
        vector<int> handednessFlags(count, 0);

        for (size_t i = 0; i < asset.m_indices.size(); i += 3)
        {
            const size_t indices[3] = { asset.m_indices[i], asset.m_indices[i + 1], asset.m_indices[i + 2]};

            AIKO_ASSERT(indices[0] < count && indices[1] < count && indices[2] < count, "Mesh tangent generation encountered an invalid index");

            const vec3& p0 = asset.m_vertices[indices[0]];
            const vec3& p1 = asset.m_vertices[indices[1]];
            const vec3& p2 = asset.m_vertices[indices[2]];

            const vec2& uv0 = asset.m_textCoord[indices[0]];
            const vec2& uv1 = asset.m_textCoord[indices[1]];
            const vec2& uv2 = asset.m_textCoord[indices[2]];

            const vec3 e1 = p1 - p0;
            const vec3 e2 = p2 - p0;

            const vec2 d1 = uv1 - uv0;
            const vec2 d2 = uv2 - uv0;

            const float det = d1.x * d2.y - d1.y * d2.x;

            if (!std::isfinite(det) || std::abs(det) <= 1e-8f)
            {
                continue;
            }

            const float areaSquared = math::lengthSquared(math::cross(e1, e2));

            if (!std::isfinite(areaSquared) || areaSquared <= 1e-12f)
            {
                continue;
            }

            vec3 T = (e1 * d2.y - e2 * d1.y) / det;
            vec3 B = (e2 * d1.x - e1 * d2.x) / det;

            const float tSquared = math::lengthSquared(T);
            const float bSquared = math::lengthSquared(B);

            if (!std::isfinite(tSquared) || !std::isfinite(bSquared) || tSquared <= 1e-12f || bSquared <= 1e-12f)
            {
                continue;
            }

            T = T / std::sqrt(tSquared);
            B = B / std::sqrt(bSquared);

            const float weight = std::sqrt(areaSquared);

            for (size_t index : indices)
            {
                const vec3& normal = asset.m_normals[index];

                const float nSquared = math::lengthSquared(normal);

                if (!std::isfinite(nSquared) || nSquared <= 1e-12f)
                {
                    continue;
                }

                const vec3 N = normal / std::sqrt(nSquared);

                const float orientation = math::dot(math::cross(N, T), B);

                if (!std::isfinite(orientation) || std::abs(orientation) <= 1e-6f)
                {
                    continue;
                }

                const int sign = orientation < 0.0f ? -1 : 1;

                if (handednessFlags[index] == 2)
                {
                    continue;
                }

                if (handednessFlags[index] != 0 && handednessFlags[index] != sign)
                {
                    handednessFlags[index] = 2;
                    continue;
                }

                handednessFlags[index] = sign;

                tangentSum[index] += T * weight;
                bitangentSum[index] += B * weight;
            }
        }

        for (size_t i = 0; i < count; ++i)
        {
            if (handednessFlags[i] == 0 || handednessFlags[i] == 2)
            {
                continue;
            }

            const vec3& normal = asset.m_normals[i];
            const float nSquared = math::lengthSquared(normal);

            if (!std::isfinite(nSquared) || nSquared <= 1e-12f)
            {
                continue;
            }

            const vec3 N = normal / std::sqrt(nSquared);

            // Gram-Schmidt orthogonalization.
            vec3 T = tangentSum[i] - N * math::dot(N, tangentSum[i]);

            const float tSquared = math::lengthSquared(T);

            if (!std::isfinite(tSquared) || tSquared <= 1e-12f)
            {
                continue;
            }

            T = T / std::sqrt(tSquared);

            const float orientation = math::dot(math::cross(N, T), bitangentSum[i]);

            if (!std::isfinite(orientation) || std::abs(orientation) <= 1e-6f)
            {
                continue;
            }

            result[i] = {
                T.x,
                T.y,
                T.z,
                orientation < 0.0f ? -1.0f : 1.0f
            };
        }

        return result;
    }
}
