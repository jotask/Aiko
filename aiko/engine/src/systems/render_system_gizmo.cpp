#include "render_system.h"

#include <aiko_types.h>
#include <math/math.h>
#include <cmath>

#include "components/transform_component.h"
#include "modules/render_module.h"
#include "models/mesh_factory.h"

namespace aiko
{

    Material& RenderSystem::resolveGizmoMaterial(Color color)
    {
        const u32 key = color.rgba();

        auto [it, inserted] = m_gizmoMaterials.try_emplace(key);

        if (inserted)
        {
            Material& material = it->second;
            material.m_shaderId = m_materialPrimitives.m_shaderId;
            material.m_baseColor = color;
            material.m_lit = false;
            material.m_useVertexColor = false;
        }

        return it->second;
    }

    void RenderSystem::renderLightGizmo(vec3 position, vec3 size, Color color)
    {
        Material& material = resolveGizmoMaterial(color);
        renderSphere(position, size, 16, &material);
    }

    void RenderSystem::renderCameraGizmo(const Camera& camera, Color color)
    {
        Material& material = resolveGizmoMaterial(color);

        const vec3 position = camera.position;
        const vec3 forward = math::normalize(camera.target - camera.position);
        const vec3 right = camera.getCameraRight();
        const vec3 up = camera.getCameraUp();

        constexpr float depth = 0.75f;
        const float halfHeight = std::tan(math::radians(camera.getFOV() * 0.5f)) * depth;
        constexpr float aspect = 16.0f / 9.0f;
        const float halfWidth = halfHeight * aspect;
        const vec3 center = position + forward * depth;
        const vec3 topLeft = center + up * halfHeight - right * halfWidth;
        const vec3 topRight = center + up * halfHeight + right * halfWidth;
        const vec3 bottomLeft = center - up * halfHeight - right * halfWidth;
        const vec3 bottomRight = center - up * halfHeight + right * halfWidth;

        // Camera origin -> frustum corners
        renderLine(position, topLeft, &material);
        renderLine(position, topRight, &material);
        renderLine(position, bottomLeft, &material);
        renderLine(position, bottomRight, &material);

        // Frustum rectangle
        renderLine(topLeft, topRight, &material);
        renderLine(topRight, bottomRight, &material);
        renderLine(bottomRight, bottomLeft, &material);
        renderLine(bottomLeft, topLeft, &material);
    }

    void RenderSystem::renderBoundsGizmo(const Bounds& bounds, Color color)
    {
        Material& material = resolveGizmoMaterial(color);

        const vec3 p000 =
        {
            bounds.min.x,
            bounds.min.y,
            bounds.min.z
        };

        const vec3 p100 =
        {
            bounds.max.x,
            bounds.min.y,
            bounds.min.z
        };

        const vec3 p010 =
        {
            bounds.min.x,
            bounds.max.y,
            bounds.min.z
        };

        const vec3 p110 =
        {
            bounds.max.x,
            bounds.max.y,
            bounds.min.z
        };

        const vec3 p001 =
        {
            bounds.min.x,
            bounds.min.y,
            bounds.max.z
        };

        const vec3 p101 =
        {
            bounds.max.x,
            bounds.min.y,
            bounds.max.z
        };

        const vec3 p011 =
        {
            bounds.min.x,
            bounds.max.y,
            bounds.max.z
        };

        const vec3 p111 =
        {
            bounds.max.x,
            bounds.max.y,
            bounds.max.z
        };

        //
        // Bottom
        //

        renderLine(p000, p100, &material);
        renderLine(p100, p101, &material);
        renderLine(p101, p001, &material);
        renderLine(p001, p000, &material);

        //
        // Top
        //

        renderLine(p010, p110, &material);
        renderLine(p110, p111, &material);
        renderLine(p111, p011, &material);
        renderLine(p011, p010, &material);

        //
        // Verticals
        //

        renderLine(p000, p010, &material);
        renderLine(p100, p110, &material);
        renderLine(p101, p111, &material);
        renderLine(p001, p011, &material);
    }

}
