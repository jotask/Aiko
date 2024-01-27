#include "light_component.h"

#include "models/game_object.h"
#include "systems/render_system.h"

namespace aiko
{
    
    LightComponent::LightComponent()
        : Component("Light")
        , color( 1.f )
        , intensity(1.f)
    {
    
    }
    
    void LightComponent::init()
    {
        m_renderSystem = gameobject->getSystem<RenderSystem>().get();
        m_light = m_renderSystem->createLight();
    }
    
    void LightComponent::update()
    {
        aiko::AikoPtr<Transform> transform = gameobject->transform();
        m_light->vector = transform->position;
        m_light->color = color;
        m_light->intensity = intensity;
        m_light->update();
    }
    
    void LightComponent::render()
    {
        m_renderSystem->add(m_light.get());
    }
    
    void LightComponent::setPrimitive(Light::Type light)
    {
        m_light->m_type = light;
        m_light->reset();
    }

}
