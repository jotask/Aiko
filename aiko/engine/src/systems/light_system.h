#pragma once

#include <vector>

#include <models/light.h>
#include <aiko_renderer.h>

#include "systems/base_system.h"

namespace aiko
{

    class RenderModule;

    class LightSystem : public BaseSystem
    {
    public:

        LightSystem();
        virtual ~LightSystem() = default;

        virtual void connect(ModuleConnector*, SystemConnector*) override;

        virtual void render() override;

        void setAmbientLight(float intensity, Color color);
        void addDirectionalLight(vec3 position, vec3 direction, float intensity, Color color);
        void addPointLight(vec3 position, float intensity, float range, Color color);
        void addSpotLight(vec3 position, float intensity, float inner, float outer, Color color);

        std::vector<LightData>& getLights() { return m_lights; }

    private:

        RenderModule* m_renderModule;

        AmbientLight m_ambient;
        std::vector<LightData> m_lights;

    };

}
