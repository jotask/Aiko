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

    void RenderSystem::drawRectangle(vec3, vec3, Color)
    {
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

    void RenderSystem::bindData(std::vector<float>& verts)
    {
    }

    void RenderSystem::calculateNormals(Data& data)
    {
    }

    void RenderSystem::calculateUvs(Data& data)
    {
    }

    void RenderSystem::bindShaderAttributes()
    {
    }

    void RenderSystem::setUniforms(Color color)
    {
    }

}
