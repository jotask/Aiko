#include "render_module_2d.h"

#include <raylib.h>

#include "config.h"

#include "modules/module_connector.h"

void RenderModule2D::connect(ModuleConnector* moduleConnector)
{
    m_sceneModule = moduleConnector->find<SceneModule>();
}

void RenderModule2D::preInit()
{

}

void RenderModule2D::init()
{
}

void RenderModule2D::postInit()
{
}

void RenderModule2D::preUpdate()
{
}

void RenderModule2D::update()
{
}

void RenderModule2D::postUpdate()
{
}

void RenderModule2D::preRender()
{

}

void RenderModule2D::render()
{
}

void RenderModule2D::postRender()
{
}
