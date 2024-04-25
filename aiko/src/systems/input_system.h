#pragma once

#include <memory>
#include <vector>

#include "aiko_types.h"
#include "systems/base_system.h"
#include "models/game_object.h"
#include "types/camera_types.h"
#include "models/camera.h"
#include "types/camera_types.h"
#include "shared/math.h"
#include "types/inputs.h"

namespace aiko
{

    class InputModule;
    
    // TODO ? DO we need this class? This is just a proxy really

    class InputSystem : public BaseSystem
    {
        typedef EnumIterator<aiko::Key, aiko::Key::KEY_UNKNOWN, aiko::Key::KEY_MENU> KeyIterator;
    public:
    
        using CameraPtr = std::shared_ptr<Camera>;

        InputSystem() = default;
        virtual ~InputSystem() = default;

        bool isKeyPressed(Key) const;
        bool isKeyJustPressed(Key) const;
        vec2 getMousePosition() const;
        bool isMouseButtonPressed(MouseButton button) const;

    protected:
    
        virtual void connect(ModuleConnector*, SystemConnector*) override;

    private:

        InputModule* m_inputModule;
    
    };

}
