#include "render_system.h"

#include <stdexcept>
#include <memory>
#include <cassert>
#include <iostream>
#include <format>

#include "aiko_types.h"
#include "shared/math.h"
#include "modules/module_connector.h"
#include "systems/system_connector.h"
#include "components/transform_component.h"
#include "components/mesh_component.h"
#include "systems/camera_system.h"
#include "models/light.h"
#include "models/mesh_factory.h"
#include "types/color.h"

namespace aiko
{

    void RenderSystem::initPrimitives()
    {
        m_quadShaderPrimitives.load("primitive");
        m_quadShaderPrimitives.preLoadUniforms({
            {"u_color", ShaderUniformDataType::SHADER_UNIFORM_VEC4},
            {"u_border", ShaderUniformDataType::SHADER_UNIFORM_FLOAT},
            {"u_border_thickness", ShaderUniformDataType::SHADER_UNIFORM_FLOAT}
        });
    }

    void RenderSystem::setPrimitiveShaderData(bool border, float thickness, Color color)
    {
        m_quadShaderPrimitives.setBool("u_border", border);
        m_quadShaderPrimitives.setFloat("u_border_thickness", thickness);
        m_quadShaderPrimitives.setVec4("u_color", { color.r, color.g, color.b, color.a });
    }

    void RenderSystem::drawPoint(vec3 pos, Color color, bool border, float thickness)
    {
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        Mesh mesh;
        mesh.m_vertices = {
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, color.r, color.g, color.b,
        };
        mesh.m_indices = { 0 };
        m_renderModule->renderTransientBuffer(camera, &t, &m_quadShaderPrimitives, &mesh.m_vertices, &mesh.m_indices);
    }

    void RenderSystem::drawTriangle(vec3 pos, vec3 size, Color color, bool border, float thickness)
    {
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        Mesh mesh;
        mesh::generateTriangle(mesh);
        m_renderModule->renderTransientBuffer(camera, &t, &m_quadShaderPrimitives, &mesh.m_vertices, &mesh.m_indices);

    }

    void RenderSystem::drawRectangle(vec3 pos, vec3 size, Color color, bool border, float thickness)
    {
        setPrimitiveShaderData(border, thickness, color);
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        t.scale = size;
        Mesh mesh;
        mesh::generateQuad(mesh, WHITE);
        m_renderModule->renderTransientBuffer(camera, &t, &m_quadShaderPrimitives, &mesh.m_vertices, &mesh.m_indices );
    }

    void RenderSystem::renderLine(vec3 start, vec3 end, Color color, bool border, float thickness)
    {
        setPrimitiveShaderData(border, thickness, color);
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        Mesh mesh;
        mesh.m_vertices = {
            start.x, start.y, start.z, 0.0f, 0.0f, color.r, color.g, color.b, // start vertex
            end.x,   end.y,   end.z,   0.0f, 0.0f, color.r, color.g, color.b  // end vertex
        };
        mesh.m_indices = { 0, 1 }; // two vertices
        m_renderModule->renderTransientBuffer(camera, &t, &m_quadShaderPrimitives, &mesh.m_vertices, &mesh.m_indices);

    }

    void RenderSystem::renderCircle(vec3 pos, vec3 size, Color color, bool border, float thickness)
    {
        setPrimitiveShaderData(border, thickness, color);
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        Mesh mesh;
        mesh::generateCircle(mesh);
        m_renderModule->renderTransientBuffer(camera, &t, &m_quadShaderPrimitives, &mesh.m_vertices, &mesh.m_indices);
    }

    void RenderSystem::renderNgon(vec3 pos, vec3 size, uint segment, Color color, bool border, float thickness)
    {
        setPrimitiveShaderData(border, thickness, color);
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        Mesh mesh;
        mesh::generateCircle(mesh, segment);
        m_renderModule->renderTransientBuffer(camera, &t, &m_quadShaderPrimitives, &mesh.m_vertices, &mesh.m_indices);
    }

    void RenderSystem::drawPyramid(vec3 pos, vec3 size, Color color, bool border, float thickness)
    {
        setPrimitiveShaderData(border, thickness, color);
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        Mesh mesh;
        mesh::generatePyramid(mesh);
        m_renderModule->renderTransientBuffer(camera, &t, &m_quadShaderPrimitives, &mesh.m_vertices, &mesh.m_indices);
    }

    void RenderSystem::drawCube(vec3 pos, vec3 size, Color color, bool border, float thickness)
    {
        setPrimitiveShaderData(border, thickness, color);
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        t.scale *= size;
        Mesh mesh;
        mesh::generateCube(mesh);
        m_renderModule->renderTransientBuffer(camera, &t, &m_quadShaderPrimitives, &mesh.m_vertices, &mesh.m_indices);
    }

    void RenderSystem::renderSphere(vec3 pos, vec3 size, int segments, Color color, bool border, float thickness)
    {
        setPrimitiveShaderData(border, thickness, color);
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        Mesh mesh;
        mesh::generateMeshSphere(mesh, segments, segments);
        m_renderModule->renderTransientBuffer(camera, &t, &m_quadShaderPrimitives, &mesh.m_vertices, &mesh.m_indices);
    }

    void RenderSystem::renderPolygon(vec3 pos, vec3 size, int rings, int sectors, Color color, bool border, float thickness)
    {
        setPrimitiveShaderData(border, thickness, color);
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        Mesh mesh;
        mesh::generateMeshSphere(mesh, sectors, sectors);
        m_renderModule->renderTransientBuffer(camera, &t, &m_quadShaderPrimitives, &mesh.m_vertices, &mesh.m_indices);
    }

    void RenderSystem::renderCylinder(vec3 pos, vec3 size, uint sectors, Color color, bool border, float thickness)
    {
        setPrimitiveShaderData(border, thickness, color);
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        Mesh mesh;
        mesh::generateMeshCylinder(mesh, sectors);
        m_renderModule->renderTransientBuffer(camera, &t, &m_quadShaderPrimitives, &mesh.m_vertices, &mesh.m_indices);
    }

    void RenderSystem::renderTorus(vec3 pos, vec3 size, Color color, bool border, float thickness)
    {
        setPrimitiveShaderData(border, thickness, color);
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        Mesh mesh;
        mesh::generateMeshTorus(mesh);
        m_renderModule->renderTransientBuffer(camera, &t, &m_quadShaderPrimitives, &mesh.m_vertices, &mesh.m_indices);
    }

    void RenderSystem::renderKnot(vec3 pos, vec3 size, Color color, bool border, float thickness)
    {
        setPrimitiveShaderData(border, thickness, color);
        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        Mesh mesh;
        mesh::generateMeshKnot(mesh);
        m_renderModule->renderTransientBuffer(camera, &t, &m_quadShaderPrimitives, &mesh.m_vertices, &mesh.m_indices);
    }

}
