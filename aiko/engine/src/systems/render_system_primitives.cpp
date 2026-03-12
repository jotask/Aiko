#include "render_system.h"

#include <aiko_types.h>
#include <math/math.h>

#include "components/transform_component.h"
#include "modules/render_module.h"

namespace aiko
{
    Material& RenderSystem::resolvePrimitiveMaterial(Material* material)
    {
        return (material == nullptr) ? m_materialPrimitives : *material;
    }

    void RenderSystem::renderPoint(vec3 pos, Material* material)
    {
        Transform t;
        t.position = pos;
        Mesh& mesh = m_primitiveMeshCache.getPointMesh();
        m_renderModule->getRenderer().submit(t, mesh, resolvePrimitiveMaterial(material));
    }

    void RenderSystem::renderTriangle(vec3 pos, vec3 size, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        Mesh& mesh = m_primitiveMeshCache.getTriangleMesh();
        m_renderModule->getRenderer().submit(t, mesh, resolvePrimitiveMaterial(material));
    }

    void RenderSystem::renderRectangle(vec3 pos, vec3 size, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        Mesh& mesh = m_primitiveMeshCache.getRectangleMesh();
        m_renderModule->getRenderer().submit(t, mesh, resolvePrimitiveMaterial(material));
    }

    void RenderSystem::renderLine(vec3 start, vec3 end, Material* material)
    {
        float thickness = 0.25f;
        const vec3 dir = end - start;
        Transform t;
        t.position = start;
        t.rotation = vec3(0.0f,math::degrees(atan2f(-dir.z, dir.x)),math::degrees(asin(math::clamp(dir.y, -1.0f, 0.0f))));
        t.scale = vec3(math::length(dir), thickness, thickness);
        Mesh& mesh = m_primitiveMeshCache.getLineMesh();
        m_renderModule->getRenderer().submit(t, mesh, resolvePrimitiveMaterial(material));
    }

    void RenderSystem::renderCircle(vec3 pos, vec3 size, uint segments, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        Mesh& mesh = m_primitiveMeshCache.getOrCreateCircleMesh(segments);
        m_renderModule->getRenderer().submit(t, mesh, resolvePrimitiveMaterial(material));
    }

    void RenderSystem::renderNgon(vec3 pos, vec3 size, uint segment, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        Mesh& mesh = m_primitiveMeshCache.getOrCreateCircleMesh(segment);
        m_renderModule->getRenderer().submit(t, mesh, resolvePrimitiveMaterial(material));
    }

    void RenderSystem::renderGrid(vec3 pos, vec3 size, ivec2 resolution, Material* material)
    {
        if (resolution.x < 2 || resolution.y < 2)
        {
            return;
        }
        Transform t;
        t.position = pos;
        t.scale = size;
        Mesh& mesh = m_primitiveMeshCache.getOrCreateGridMesh(resolution);
        m_renderModule->getRenderer().submit(t, mesh, resolvePrimitiveMaterial(material));
    }

    void RenderSystem::renderPyramid(vec3 pos, vec3 size, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        Mesh& mesh = m_primitiveMeshCache.getPyramidMesh();
        m_renderModule->getRenderer().submit(t, mesh, resolvePrimitiveMaterial(material));
    }

    void RenderSystem::renderCube(vec3 pos, vec3 size, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        Mesh& mesh = m_primitiveMeshCache.getCubeMesh();
        m_renderModule->getRenderer().submit(t, mesh, resolvePrimitiveMaterial(material));
    }

    void RenderSystem::renderSphere(vec3 pos, vec3 size, int segments, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        Mesh& mesh = m_primitiveMeshCache.getOrCreateSphereMesh(segments, segments);
        m_renderModule->getRenderer().submit(t, mesh, resolvePrimitiveMaterial(material));
    }

    void RenderSystem::renderPolygon(vec3 pos, vec3 size, int rings, int sectors, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        Mesh& mesh = m_primitiveMeshCache.getOrCreateSphereMesh(rings, sectors);
        m_renderModule->getRenderer().submit(t, mesh, resolvePrimitiveMaterial(material));
    }

    void RenderSystem::renderCylinder(vec3 pos, vec3 size, uint sectors, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        Mesh& mesh = m_primitiveMeshCache.getOrCreateCylinderMesh(sectors);
        m_renderModule->getRenderer().submit(t, mesh, resolvePrimitiveMaterial(material));
    }

    void RenderSystem::renderTorus(vec3 pos, vec3 size, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        Mesh& mesh = m_primitiveMeshCache.getTorusMesh();
        m_renderModule->getRenderer().submit(t, mesh, resolvePrimitiveMaterial(material));
    }

    void RenderSystem::renderKnot(vec3 pos, vec3 size, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        Mesh& mesh = m_primitiveMeshCache.getKnotMesh();
        m_renderModule->getRenderer().submit(t, mesh, resolvePrimitiveMaterial(material));
    }

}
