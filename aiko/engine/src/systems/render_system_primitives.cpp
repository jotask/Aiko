#include "render_system.h"

#include <aiko_types.h>
#include <math/math.h>

#include "components/transform_component.h"
#include "modules/render_module.h"
#include "models/mesh_factory.h"

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
        static MeshAsset mesh = mesh::factory::generatePoint();
        m_renderModule->getRenderer().submitTransient(t, resolvePrimitiveMaterial(material), mesh, TransientTopology::Points);
    }

    void RenderSystem::renderTriangle(vec3 pos, vec3 size, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        MeshAsset& mesh = m_primitiveMeshCache.getTriangleMesh();
        m_renderModule->getRenderer().submitTransient(t, resolvePrimitiveMaterial(material), mesh, m_defaultTransientTopology);
    }

    void RenderSystem::renderRectangle(vec3 pos, vec3 size, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        MeshAsset& mesh = m_primitiveMeshCache.getRectangleMesh();
        m_renderModule->getRenderer().submitTransient(t, resolvePrimitiveMaterial(material), mesh, m_defaultTransientTopology);
    }

    void RenderSystem::renderLine(vec3 start, vec3 end, Material* material)
    {
        Transform t;
        MeshAsset mesh = mesh::factory::generateLine(start, end);
        m_renderModule->getRenderer().submitTransient(t, resolvePrimitiveMaterial(material), mesh, TransientTopology::Lines);
    }

    void RenderSystem::renderCircle(vec3 pos, vec3 size, uint segments, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        MeshAsset& mesh = m_primitiveMeshCache.getOrCreateCircleMesh(segments);
        m_renderModule->getRenderer().submitTransient(t, resolvePrimitiveMaterial(material), mesh, m_defaultTransientTopology);
    }

    void RenderSystem::renderNgon(vec3 pos, vec3 size, uint segment, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        MeshAsset& mesh = m_primitiveMeshCache.getOrCreateCircleMesh(segment);
        m_renderModule->getRenderer().submitTransient(t, resolvePrimitiveMaterial(material), mesh, m_defaultTransientTopology);
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
        m_renderModule->getRenderer().submitTransient(t, resolvePrimitiveMaterial(material), mesh, m_defaultTransientTopology);
    }

    void RenderSystem::renderPyramid(vec3 pos, vec3 size, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        MeshAsset& mesh = m_primitiveMeshCache.getPyramidMesh();
        m_renderModule->getRenderer().submitTransient(t, resolvePrimitiveMaterial(material), mesh, m_defaultTransientTopology);
    }

    void RenderSystem::renderCube(vec3 pos, vec3 size, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        MeshAsset& mesh = m_primitiveMeshCache.getCubeMesh();
        m_renderModule->getRenderer().submitTransient(t, resolvePrimitiveMaterial(material), mesh, m_defaultTransientTopology);
    }

    void RenderSystem::renderSphere(vec3 pos, vec3 size, int segments, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        MeshAsset& mesh = m_primitiveMeshCache.getOrCreateSphereMesh(segments, segments);
        m_renderModule->getRenderer().submitTransient(t, resolvePrimitiveMaterial(material), mesh, m_defaultTransientTopology);
    }

    void RenderSystem::renderPolygon(vec3 pos, vec3 size, int rings, int sectors, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        MeshAsset& mesh = m_primitiveMeshCache.getOrCreateSphereMesh(rings, sectors);
        m_renderModule->getRenderer().submitTransient(t, resolvePrimitiveMaterial(material), mesh, m_defaultTransientTopology);
    }

    void RenderSystem::renderCylinder(vec3 pos, vec3 size, uint sectors, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        MeshAsset& mesh = m_primitiveMeshCache.getOrCreateCylinderMesh(sectors);
        m_renderModule->getRenderer().submitTransient(t, resolvePrimitiveMaterial(material), mesh, m_defaultTransientTopology);
    }

    void RenderSystem::renderTorus(vec3 pos, vec3 size, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        MeshAsset& mesh = m_primitiveMeshCache.getTorusMesh();
        m_renderModule->getRenderer().submitTransient(t, resolvePrimitiveMaterial(material), mesh, m_defaultTransientTopology);
    }

    void RenderSystem::renderKnot(vec3 pos, vec3 size, Material* material)
    {
        Transform t;
        t.position = pos;
        t.scale = size;
        MeshAsset& mesh = m_primitiveMeshCache.getKnotMesh();
        m_renderModule->getRenderer().submitTransient(t, resolvePrimitiveMaterial(material), mesh, m_defaultTransientTopology);
    }

}
