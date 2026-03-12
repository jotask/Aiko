#include "render_system.h"

#include <aiko_types.h>
#include <math/math.h>

#include "components/transform_component.h"
#include "modules/render_module.h"

namespace aiko
{

    void RenderSystem::renderPoint(vec3 pos)
    {
        Transform t;
        t.position = pos;
        Mesh& mesh = m_primitiveMeshCache.getPointMesh();
        m_renderModule->getRenderer().submit(t, mesh, m_materialPrimitives);
    }

    void RenderSystem::renderTriangle(vec3 pos, vec3 size)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        Mesh& mesh = m_primitiveMeshCache.getTriangleMesh();
        m_renderModule->getRenderer().submit(t, mesh, m_materialPrimitives);
    }

    void RenderSystem::renderRectangle(vec3 pos, vec3 size)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        Mesh& mesh = m_primitiveMeshCache.getRectangleMesh();
        m_renderModule->getRenderer().submit(t, mesh, m_materialPrimitives);
    }

    void RenderSystem::renderLine(vec3 start, vec3 end)
    {
        Transform t;
        Mesh& mesh = m_primitiveMeshCache.getOrCreateLineMesh(start, end);
        m_renderModule->getRenderer().submit(t, mesh, m_materialPrimitives);
    }

    void RenderSystem::renderCircle(vec3 pos, vec3 size, uint segments)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        Mesh& mesh = m_primitiveMeshCache.getOrCreateCircleMesh(segments);
        m_renderModule->getRenderer().submit(t, mesh, m_materialPrimitives);
    }

    void RenderSystem::renderNgon(vec3 pos, vec3 size, uint segment)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        Mesh& mesh = m_primitiveMeshCache.getOrCreateCircleMesh(segment);
        m_renderModule->getRenderer().submit(t, mesh, m_materialPrimitives);
    }

    void RenderSystem::renderGrid(vec3 pos, vec3 size, ivec2 resolution)
    {
        if (resolution.x < 2 || resolution.y < 2)
        {
            return;
        }
        Transform t;
        t.position = pos;
        t.scale = size;
        Mesh& mesh = m_primitiveMeshCache.getOrCreateGridMesh(resolution);
        m_renderModule->getRenderer().submit(t, mesh, m_materialPrimitives);
    }

    void RenderSystem::renderPyramid(vec3 pos, vec3 size)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        Mesh& mesh = m_primitiveMeshCache.getPyramidMesh();
        m_renderModule->getRenderer().submit(t, mesh, m_materialPrimitives);
    }

    void RenderSystem::renderCube(vec3 pos, vec3 size)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        Mesh& mesh = m_primitiveMeshCache.getCubeMesh();
        m_renderModule->getRenderer().submit(t, mesh, m_materialPrimitives);
    }

    void RenderSystem::renderSphere(vec3 pos, vec3 size, int segments)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        Mesh& mesh = m_primitiveMeshCache.getOrCreateSphereMesh(segments, segments);
        m_renderModule->getRenderer().submit(t, mesh, m_materialPrimitives);
    }

    void RenderSystem::renderPolygon(vec3 pos, vec3 size, int rings, int sectors)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        Mesh& mesh = m_primitiveMeshCache.getOrCreateSphereMesh(rings, sectors);
        m_renderModule->getRenderer().submit(t, mesh, m_materialPrimitives);
    }

    void RenderSystem::renderCylinder(vec3 pos, vec3 size, uint sectors)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        Mesh& mesh = m_primitiveMeshCache.getOrCreateCylinderMesh(sectors);
        m_renderModule->getRenderer().submit(t, mesh, m_materialPrimitives);
    }

    void RenderSystem::renderTorus(vec3 pos, vec3 size)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        Mesh& mesh = m_primitiveMeshCache.getTorusMesh();
        m_renderModule->getRenderer().submit(t, mesh, m_materialPrimitives);
    }

    void RenderSystem::renderKnot(vec3 pos, vec3 size)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        Mesh& mesh = m_primitiveMeshCache.getKnotMesh();
        m_renderModule->getRenderer().submit(t, mesh, m_materialPrimitives);
    }

}
