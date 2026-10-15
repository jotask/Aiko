#include "render_system.h"

#include <aiko_types.h>
#include <math/math.h>

#include "components/transform_component.h"
#include "modules/render_module.h"
#include "models/mesh_factory.h"

namespace aiko
{
    Material RenderSystem::makePrimitiveMaterial(Color color) const
    {
        Material material = m_materialPrimitives;

        material.m_baseColor = color;
        material.m_useVertexColor = false;
        material.m_lit = false;

        material.m_renderState.blend = color.a < 1.0f;
        material.m_renderState.depthWrite = !material.m_renderState.blend;

        return material;
    }

    Material& RenderSystem::resolvePrimitiveMaterial(Material* material)
    {
        return (material == nullptr) ? m_materialPrimitives : *material;
    }

    void RenderSystem::renderPoint(vec3 pos, Color color)
    {
        Material material = makePrimitiveMaterial(color);
        renderPoint(pos, &material);
    }

    void RenderSystem::renderLine(vec3 start, vec3 end, Color color)
    {
        Material material = makePrimitiveMaterial(color);
        renderLine(start, end, &material);
    }

    void RenderSystem::renderTriangle(vec3 pos, vec3 size, Color color)
    {
        Material material = makePrimitiveMaterial(color);
        renderTriangle(pos, size, &material);
    }

    void RenderSystem::renderRectangle(vec3 pos, vec3 size, Color color)
    {
        Material material = makePrimitiveMaterial(color);
        renderRectangle(pos, size, &material);
    }

    void RenderSystem::renderCircle(vec3 pos, vec3 size, uint segments, Color color)
    {
        Material material = makePrimitiveMaterial(color);
        renderCircle(pos, size, segments, &material);
    }

    void RenderSystem::renderNgon(vec3 pos, vec3 size, uint polygons, Color color)
    {
        Material material = makePrimitiveMaterial(color);
        renderNgon(pos, size, polygons, &material);
    }

    void RenderSystem::renderGrid(vec3 pos, vec3 size, ivec2 resolution, Color color)
    {
        Material material = makePrimitiveMaterial(color);
        renderGrid(pos, size, resolution, &material);
    }

    void RenderSystem::renderPyramid(vec3 pos, vec3 size, Color color)
    {
        Material material = makePrimitiveMaterial(color);
        renderPyramid(pos, size, &material);
    }

    void RenderSystem::renderCube(vec3 pos, vec3 size, Color color)
    {
        Material material = makePrimitiveMaterial(color);
        renderCube(pos, size, &material);
    }

    void RenderSystem::renderSphere(vec3 pos, vec3 size, Color color, int segments)
    {
        Material material = makePrimitiveMaterial(color);
        renderSphere(pos, size, segments, &material);
    }

    void RenderSystem::renderPolygon(vec3 pos, vec3 size, int rings, int sectors, Color color)
    {
        Material material = makePrimitiveMaterial(color);
        renderPolygon(pos, size, rings, sectors, &material);
    }

    void RenderSystem::renderCylinder(vec3 pos, vec3 size, uint sectors, Color color)
    {
        Material material = makePrimitiveMaterial(color);
        renderCylinder(pos, size, sectors, &material);
    }

    void RenderSystem::renderTorus(vec3 pos, vec3 size, Color color)
    {
        Material material = makePrimitiveMaterial(color);
        renderTorus(pos, size, &material);
    }

    void RenderSystem::renderKnot(vec3 pos, vec3 size, Color color)
    {
        Material material = makePrimitiveMaterial(color);
        renderKnot(pos, size, &material);
    }

    void RenderSystem::renderPoint(vec3 pos, Material* material)
    {
        Transform t;
        t.position = pos;
        static MeshAsset mesh = mesh::factory::generatePoint();
        m_renderModule->submitTransient(t, resolvePrimitiveMaterial(material), mesh, TransientTopology::Points);
    }

    void RenderSystem::renderTriangle(vec3 pos, vec3 size, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        MeshAsset& mesh = m_primitiveMeshCache.getTriangleMesh();
        m_renderModule->submitTransient(t, resolvePrimitiveMaterial(material), mesh, m_defaultTransientTopology);
    }

    void RenderSystem::renderRectangle(vec3 pos, vec3 size, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        MeshAsset& mesh = m_primitiveMeshCache.getRectangleMesh();
        m_renderModule->submitTransient(t, resolvePrimitiveMaterial(material), mesh, m_defaultTransientTopology);
    }

    void RenderSystem::renderLine(vec3 start, vec3 end, Material* material)
    {
        Transform t;
        MeshAsset mesh = mesh::factory::generateLine(start, end);
        m_renderModule->submitTransient(t, resolvePrimitiveMaterial(material), mesh, TransientTopology::Lines);
    }

    void RenderSystem::renderCircle(vec3 pos, vec3 size, uint segments, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        MeshAsset& mesh = m_primitiveMeshCache.getOrCreateCircleMesh(segments);
        m_renderModule->submitTransient(t, resolvePrimitiveMaterial(material), mesh, m_defaultTransientTopology);
    }

    void RenderSystem::renderNgon(vec3 pos, vec3 size, uint segment, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        MeshAsset& mesh = m_primitiveMeshCache.getOrCreateCircleMesh(segment);
        m_renderModule->submitTransient(t, resolvePrimitiveMaterial(material), mesh, m_defaultTransientTopology);
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
        MeshAsset& mesh = m_primitiveMeshCache.getOrCreateGridMesh(resolution);
        m_renderModule->submitTransient(t, resolvePrimitiveMaterial(material), mesh, m_defaultTransientTopology);
    }

    void RenderSystem::renderPyramid(vec3 pos, vec3 size, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        MeshAsset& mesh = m_primitiveMeshCache.getPyramidMesh();
        m_renderModule->submitTransient(t, resolvePrimitiveMaterial(material), mesh, m_defaultTransientTopology);
    }

    void RenderSystem::renderCube(vec3 pos, vec3 size, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        MeshAsset& mesh = m_primitiveMeshCache.getCubeMesh();
        m_renderModule->submitTransient(t, resolvePrimitiveMaterial(material), mesh, m_defaultTransientTopology);
    }

    void RenderSystem::renderSphere(vec3 pos, vec3 size, int segments, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        MeshAsset& mesh = m_primitiveMeshCache.getOrCreateSphereMesh(segments, segments);
        m_renderModule->submitTransient(t, resolvePrimitiveMaterial(material), mesh, m_defaultTransientTopology);
    }

    void RenderSystem::renderPolygon(vec3 pos, vec3 size, int rings, int sectors, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        MeshAsset& mesh = m_primitiveMeshCache.getOrCreateSphereMesh(rings, sectors);
        m_renderModule->submitTransient(t, resolvePrimitiveMaterial(material), mesh, m_defaultTransientTopology);
    }

    void RenderSystem::renderCylinder(vec3 pos, vec3 size, uint sectors, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        MeshAsset& mesh = m_primitiveMeshCache.getOrCreateCylinderMesh(sectors);
        m_renderModule->submitTransient(t, resolvePrimitiveMaterial(material), mesh, m_defaultTransientTopology);
    }

    void RenderSystem::renderTorus(vec3 pos, vec3 size, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        MeshAsset& mesh = m_primitiveMeshCache.getTorusMesh();
        m_renderModule->submitTransient(t, resolvePrimitiveMaterial(material), mesh, m_defaultTransientTopology);
    }

    void RenderSystem::renderKnot(vec3 pos, vec3 size, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        MeshAsset& mesh = m_primitiveMeshCache.getKnotMesh();
        m_renderModule->submitTransient(t, resolvePrimitiveMaterial(material), mesh, m_defaultTransientTopology);
    }
}
