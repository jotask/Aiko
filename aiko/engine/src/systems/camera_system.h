#pragma once

#include "systems/base_system.h"

namespace aiko
{
    class InputSystem;
    class SceneSystem;
    class CameraComponent;

    class CameraSystem : public BaseSystem
    {
    public:
        CameraSystem() = default;
        virtual ~CameraSystem() override = default;

    protected:
        virtual void connect(ModuleConnector*, SystemConnector*) override;
        virtual void update() override;

    private:
        void updateCamera(CameraComponent& camera);

        InputSystem* m_inputSystem = nullptr;
        SceneSystem* m_sceneSystem = nullptr;
    };
}
