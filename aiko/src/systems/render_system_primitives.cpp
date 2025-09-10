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
        mesh::generateQuad(m_quadMesh, WHITE);
        refreshMesh(&m_quadMesh);
        m_quadShader.load("primitive");
        m_quadShader.preLoadUniforms({
            {"u_color", ShaderUniformDataType::SHADER_UNIFORM_VEC4},
            {"u_border", ShaderUniformDataType::SHADER_UNIFORM_FLOAT },
            {"u_border_thickness", ShaderUniformDataType::SHADER_UNIFORM_FLOAT}
            });
    }

    void RenderSystem::drawPoint(vec3, Color)
    {
    }

    void RenderSystem::drawTriangle(vec3, float, Color)
    {
    }

    void RenderSystem::drawTriangle(vec3, vec3, vec3, Color)
    {
    }

    void RenderSystem::drawRectangle(vec3 pos, vec3 size, Color color, bool border, float border_thickness)
    {

        m_quadShader.setBool("u_border", border);
        m_quadShader.setFloat("u_border_thickness", border_thickness);
        m_quadShader.setVec4("u_color", { color.r, color.g, color. b, color.a});

        aiko::Camera* camera = m_cameraSystem->getMainCamera();
        Transform t;
        t.position = pos;
        t.scale *= size;
        m_renderModule->renderMesh(camera, &t, &m_quadMesh, &m_quadShader );
    }

    void RenderSystem::renderLine(vec3, vec3, Color)
    {
    }

    void RenderSystem::renderCircle(vec3, float, int, Color)
    {
    }

    void RenderSystem::renderNgon(vec3, float, unsigned int, Color)
    {
    }

    void RenderSystem::drawPyramid(vec3, float baseWidth, float height, Color)
    {
    }

    void RenderSystem::drawCube(vec3, vec3, Color)
    {
    }

    void RenderSystem::renderSphere(vec3, float, int, Color)
    {
    }

    void RenderSystem::renderPolygon(vec3, float, int rings, int sectors, Color)
    {
    }

    void RenderSystem::renderCylinder(vec3 pos, float radius, float height, int sectors, Color)
    {
    }

}
