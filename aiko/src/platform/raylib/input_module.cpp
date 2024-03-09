#ifdef AIKO_BUILD_RAYLIB
#include "modules/input_module.h"

#include "models/time.h"

#include "aiko_types.h"
#include "core/libs.h"
#include "platform/raylib/raylib_utils.h"

namespace aiko
{

    bool InputModule::isKeyPressed(Key key) const
    {
        return ::IsKeyPressed((::KeyboardKey)key);
    }

    vec2 InputModule::getMousePosition() const
    {
        auto tmp = ::GetMousePosition();
        return raylib::utils::toVec2(tmp);
    }

    bool InputModule::isMouseButtonPressed(MouseButton button) const
    {
        return ::IsMouseButtonPressed((::MouseButton)button);
    }

    void InputModule::init()
    {
    }

    void InputModule::preUpdate()
    {

    }

    void InputModule::beginFrame()
    {

    }

    void InputModule::endFrame()
    {
        auto& time = Time::it();
        time.deltaTime = ::GetFrameTime();
        time.fps = ::GetFPS();
        time.frames++;
    }

}
#endif
