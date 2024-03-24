#pragma once

#include "models/component.h"
#include "models/light.h"
#include "types/render_types.h"

namespace aiko
{
    
    class RenderSystem;
    
    class LightComponent : public Component, public IUpdate, public IRender
    {
    public:
        LightComponent();
        virtual ~LightComponent() = default;
    
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
    
        void setPrimitive(Light::Type);
    
        Color color;
        float intensity;
    
    // private:
        RenderSystem* m_renderSystem;
        aiko::AikoPtr<Light> m_light;
    
    };

}
