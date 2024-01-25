#pragma once

#include <glm/glm.hpp>

#include "models/component.h"
#include "models/light.h"

class RenderSystem;

class LightComponent : public Component
{
public:
    LightComponent();
    virtual ~LightComponent() = default;

    virtual void init() override;
    virtual void update() override;
    virtual void render() override;

    void setPrimitive(Light::Type);

    glm::vec4 color;
    float intensity;

// private:
    RenderSystem* m_renderSystem;
    aiko::AikoPtr<Light> m_light;

};