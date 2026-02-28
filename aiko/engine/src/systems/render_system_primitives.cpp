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
#include "models/mesh_factory.h"

namespace aiko
{

    void RenderSystem::drawPoint(vec3 pos, Color color, bool border, float thickness)
    {
        Transform t;
        t.position = pos;
        static const Mesh mesh(mesh::factory::generatePoint());
        AikoRenderer::it().submit(t, mesh, m_materialPrimitives);
    }

    void RenderSystem::drawTriangle(vec3 pos, vec3 size, Color color, bool border, float thickness)
    {
        Transform t;
        t.position = pos;
        static const Mesh mesh(mesh::factory::generateTriangle());
        AikoRenderer::it().submit(t, mesh, m_materialPrimitives);
    }

    void RenderSystem::drawRectangle(vec3 pos, vec3 size, Color color, bool border, float thickness)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        static const Mesh mesh(mesh::factory::generateQuad());
        AikoRenderer::it().submit(t, mesh, m_materialPrimitives);
    }

    void RenderSystem::renderLine(vec3 start, vec3 end, Color color, bool border, float thickness)
    {
        Transform t;
        static const Mesh mesh(mesh::factory::generateLine(start, end));
        AikoRenderer::it().submit(t, mesh, m_materialPrimitives);
    }

    void RenderSystem::renderCircle(vec3 pos, vec3 size, Color color, bool border, float thickness)
    {
        Transform t;
        t.position = pos;
        static const Mesh mesh(mesh::factory::generateCircle());
        AikoRenderer::it().submit(t, mesh, m_materialPrimitives);
    }

    void RenderSystem::renderNgon(vec3 pos, vec3 size, uint segment, Color color, bool border, float thickness)
    {
        Transform t;
        t.position = pos;
        static const Mesh mesh(mesh::factory::generateCircle(segment));
        AikoRenderer::it().submit(t, mesh, m_materialPrimitives);
    }

    void RenderSystem::drawPlane(vec3 pos, vec3 size, Color color, bool border, float thickness)
    {
        Transform t;
        t.position = pos;
        const float ssize = 10.0f;
        const int resolution = 5;
        static const Mesh mesh(mesh::factory::generateMeshPlane(ssize, ssize, resolution, resolution));
        AikoRenderer::it().submit(t, mesh, m_materialPrimitives);
    }

    void RenderSystem::drawPyramid(vec3 pos, vec3 size, Color color, bool border, float thickness)
    {
        Transform t;
        t.position = pos;
        static const Mesh mesh(mesh::factory::generatePyramid());
        AikoRenderer::it().submit(t, mesh, m_materialPrimitives);
    }

    void RenderSystem::drawCube(vec3 pos, vec3 size, Color color, bool border, float thickness)
    {
        Transform t;
        t.position = pos;
        t.scale *= size;
        static const Mesh mesh(mesh::factory::generateCube());
        AikoRenderer::it().submit(t, mesh, m_materialPrimitives);
    }

    void RenderSystem::renderSphere(vec3 pos, vec3 size, int segments, Color color, bool border, float thickness)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        auto c = m_materialPrimitives.m_baseColor;
        auto l = m_materialPrimitives.m_lit;
        m_materialPrimitives.m_baseColor = color;
        m_materialPrimitives.m_lit = false;
        static const Mesh mesh(mesh::factory::generateMeshSphere(segments, segments));
        AikoRenderer::it().submit(t, mesh, m_materialPrimitives);
        m_materialPrimitives.m_baseColor = c;
        m_materialPrimitives.m_lit = l;
    }

    void RenderSystem::renderPolygon(vec3 pos, vec3 size, int rings, int sectors, Color color, bool border, float thickness)
    {
        Transform t;
        t.position = pos;
        static const Mesh mesh(mesh::factory::generateMeshSphere(sectors, sectors));
        AikoRenderer::it().submit(t, mesh, m_materialPrimitives);
    }

    void RenderSystem::renderCylinder(vec3 pos, vec3 size, uint sectors, Color color, bool border, float thickness)
    {
        Transform t;
        t.position = pos;
        static const Mesh mesh(mesh::factory::generateMeshCylinder(sectors));
        AikoRenderer::it().submit(t, mesh, m_materialPrimitives);
    }

    void RenderSystem::renderTorus(vec3 pos, vec3 size, Color color, bool border, float thickness)
    {
        Transform t;
        t.position = pos;
        static const Mesh mesh(mesh::factory::generateMeshTorus());
        AikoRenderer::it().submit(t, mesh, m_materialPrimitives);
    }

    void RenderSystem::renderKnot(vec3 pos, vec3 size, Color color, bool border, float thickness)
    {
        Transform t;
        t.position = pos;
        static const Mesh mesh(mesh::factory::generateMeshKnot());
        AikoRenderer::it().submit(t, mesh, m_materialPrimitives);
    }

}
