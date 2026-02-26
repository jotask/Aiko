#include "light_system.h"

#include "modules/module_connector.h"
#include "modules/render_module.h"

namespace aiko
{
    LightSystem::LightSystem()
        : m_ambient({1.0f, WHITE})
        , m_lights(MAX_LIGHTS)
    {
    }

    void LightSystem::connect(ModuleConnector* moduleConnector, SystemConnector* system_connector)
    {
        BIND_MODULE_REQUIRED(RenderModule, moduleConnector, m_renderModule)
    }

    void LightSystem::render()
    {
        m_renderModule->submitLights(m_ambient, m_lights);
    }

    void LightSystem::setAmbientLight(float intensity, Color color)
    {
        m_ambient =
        {
            .intensity = intensity,
            .color = color,
        };
    }

    void LightSystem::addDirectionalLight(vec3 position, vec3 direction, float intensity, Color color)
    {
        m_lights.push_back({
            .type = LightType::Directional,
            .position = position,
            .direction = direction,
            .color = color,
            .intensity = intensity
        });
    }

    void LightSystem::addPointLight(vec3 position, float intensity, float range, Color color)
    {
        m_lights.push_back({
            .type = LightType::Point,
            .position = position,
            .direction = vec3(1.0f),
            .color = color,
            .intensity = intensity,
            .range = range
        });
    }

    void LightSystem::addSpotLight(vec3 position, float intensity, float inner, float outer, Color color)
    {
        m_lights.push_back({
            .type = LightType::Spot,
            .position = position,
            .direction = vec3(1.0f),
            .color = color,
            .intensity = intensity,
            .range = 0.0f,
            .innerCos = inner,
            .outerCos = outer
        });
    }
}
