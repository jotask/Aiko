#pragma once

#include <memory>

#include <entt/entt.hpp>

#include "base_module.h"

namespace aiko
{
    
    class SceneModule : public BaseModule
    {
    public:
    
        SceneModule() = default;
        virtual ~SceneModule() = default;
    
    protected:
    
        virtual void init() override;
    
        virtual void update() override;
        virtual void postUpdate() override;
    
        virtual void preRender() override;
        virtual void render() override;
        virtual void postRender() override;
    
    private:
    
        entt::registry m_registry;

    };

}
