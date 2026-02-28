#include "light_component.h"

#include "models/game_object.h"

namespace aiko
{
    
    LightComponent::LightComponent()
        : Component("Light")
        , color( WHITE )
        , intensity(1.f)
    {
    
    }
    
    void LightComponent::init()
    {

    }

    void LightComponent::update()
    {

    }

    void LightComponent::render()
    {

    }
    
    void LightComponent::setPrimitive(LightType light)
    {
        this->type = light;
    }

    void LightComponent::setDirectional(Color color, vec3 dir, float range)
    {
        setPrimitive(LightType::Directional);
        this->color = color;
        this->direction = dir;
        this->range = range;
    }

    void LightComponent::setPointLight(Color color, float range)
    {
        setPrimitive(LightType::Point);
        this->color = color;
        this->range = range;
    }

    void LightComponent::setSpot(Color color, vec3 dir, float inned, float outer)
    {
        setPrimitive(LightType::Spot);
        this->color = color;
        this->direction = dir;
        this->innerCos = inned;
        this->outerCos = outer;
    }
}
