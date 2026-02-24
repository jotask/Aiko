#include "render_system.h"

#include <stdexcept>
#include <memory>
#include <iostream>
#include <format>

#include <aiko_types.h>
#include <math/math.h>
#include <models/light.h>
#include <types/color.h>
#include <time/time.h>

#include "modules/module_connector.h"
#include "systems/system_connector.h"
#include "components/transform_component.h"
#include "components/mesh_component.h"
#include "systems/camera_system.h"
#include "models/mesh_factory.h"

namespace aiko
{

    void RenderSystem::initPrimitives()
    {
        m_quadShaderPrimitives.load("primitive");
    }

    void RenderSystem::setPrimitiveShaderData(bool border, float thickness, Color color)
    {
        m_quadShaderPrimitives.setBool("u_border", border);
        m_quadShaderPrimitives.setFloat("u_border_thickness", thickness);
        m_quadShaderPrimitives.setVec4("u_color", { color.r, color.g, color.b, color.a });
        m_quadShaderPrimitives.setFloat("u_ambient", 0.5f );
        static vec4 light_dir = { 1.0f, 1.0f, 1.0f, 1.0f };
        if constexpr (false)
        {
            static float angle = 0.0f;
            float rotationSpeed = 0.25f; // radians per second
            auto dt = ::aiko::Time::it().getDeltaTime();
            angle += rotationSpeed * dt;

            float x = cosf(angle);
            float y = cosf(angle) * sinf(angle); // keep a bit of upward direction
            float z = sinf(angle);

            light_dir = { x, y, z, 0.0f };
        }
        m_quadShaderPrimitives.setVec4("u_lightDir", light_dir);
        m_quadShaderPrimitives.setVec4("u_lightColor", { 1.0f, 1.0f, 1.0f, 1.0f });
    }

    void RenderSystem::drawPoint(vec3 pos, Color color, bool border, float thickness)
    {
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        static const Mesh mesh(mesh::factory::generatePoint());
        static const Material material;
        AikoRenderer::it().submit(t, mesh, material);
    }

    void RenderSystem::drawTriangle(vec3 pos, vec3 size, Color color, bool border, float thickness)
    {
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        static const Mesh mesh(mesh::factory::generateTriangle());
        static const Material material;
        AikoRenderer::it().submit(t, mesh, material);
    }

    void RenderSystem::drawRectangle(vec3 pos, vec3 size, Color color, bool border, float thickness)
    {
        setPrimitiveShaderData(border, thickness, color);
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        t.scale = size;
        static const Mesh mesh(mesh::factory::generateQuad());
        static const Material material;
        AikoRenderer::it().submit(t, mesh, material);
    }

    void RenderSystem::renderLine(vec3 start, vec3 end, Color color, bool border, float thickness)
    {
        setPrimitiveShaderData(border, thickness, color);
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        static const Mesh mesh(mesh::factory::generateLine(start, end));
        static const Material material;
        AikoRenderer::it().submit(t, mesh, material);
    }

    void RenderSystem::renderCircle(vec3 pos, vec3 size, Color color, bool border, float thickness)
    {
        setPrimitiveShaderData(border, thickness, color);
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        static const Mesh mesh(mesh::factory::generateCircle());
        static const Material material;
        AikoRenderer::it().submit(t, mesh, material);
    }

    void RenderSystem::renderNgon(vec3 pos, vec3 size, uint segment, Color color, bool border, float thickness)
    {
        setPrimitiveShaderData(border, thickness, color);
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        static const Mesh mesh(mesh::factory::generateCircle(segment));
        static const Material material;
        AikoRenderer::it().submit(t, mesh, material);
    }

    void RenderSystem::drawPlane(vec3 pos, vec3 size, Color color, bool border, float thickness)
    {
        setPrimitiveShaderData(border, thickness, color);
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        float siz = 10.0f;
        int res = 5;
        static const Mesh mesh(mesh::factory::generateMeshPlane(siz, siz, res, res));
        static const Material material;
        AikoRenderer::it().submit(t, mesh, material);
    }

    void RenderSystem::drawPyramid(vec3 pos, vec3 size, Color color, bool border, float thickness)
    {
        setPrimitiveShaderData(border, thickness, color);
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        static const Mesh mesh(mesh::factory::generatePyramid());
        static const Material material;
        AikoRenderer::it().submit(t, mesh, material);
    }

    void RenderSystem::drawCube(vec3 pos, vec3 size, Color color, bool border, float thickness)
    {
        setPrimitiveShaderData(border, thickness, color);
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        t.scale *= size;
        static const Mesh mesh(mesh::factory::generateCube());
        static const Material material;
        AikoRenderer::it().submit(t, mesh, material);
    }

    void RenderSystem::renderSphere(vec3 pos, vec3 size, int segments, Color color, bool border, float thickness)
    {
        setPrimitiveShaderData(border, thickness, color);
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        static const Mesh mesh(mesh::factory::generateMeshSphere(segments, segments));
        static const Material material;
        AikoRenderer::it().submit(t, mesh, material);
    }

    void RenderSystem::renderPolygon(vec3 pos, vec3 size, int rings, int sectors, Color color, bool border, float thickness)
    {
        setPrimitiveShaderData(border, thickness, color);
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        static const Mesh mesh(mesh::factory::generateMeshSphere(sectors, sectors));
        static const Material material;
        AikoRenderer::it().submit(t, mesh, material);
    }

    void RenderSystem::renderCylinder(vec3 pos, vec3 size, uint sectors, Color color, bool border, float thickness)
    {
        setPrimitiveShaderData(border, thickness, color);
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        static const Mesh mesh(mesh::factory::generateMeshCylinder(sectors));
        static const Material material;
        AikoRenderer::it().submit(t, mesh, material);
    }

    void RenderSystem::renderTorus(vec3 pos, vec3 size, Color color, bool border, float thickness)
    {
        setPrimitiveShaderData(border, thickness, color);
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        static const Mesh mesh(mesh::factory::generateMeshTorus());
        static const Material material;
        AikoRenderer::it().submit(t, mesh, material);
    }

    void RenderSystem::renderKnot(vec3 pos, vec3 size, Color color, bool border, float thickness)
    {
        setPrimitiveShaderData(border, thickness, color);
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        static const Mesh mesh(mesh::factory::generateMeshKnot());
        static const Material material;
        AikoRenderer::it().submit(t, mesh, material);
    }

}
