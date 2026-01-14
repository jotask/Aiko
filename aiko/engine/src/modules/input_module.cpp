#include "modules/input_module.h"

#include <input/aiko_input.h>

namespace aiko
{
    void InputModule::init()
    {
        AikoInput::it().init();
    }

    void InputModule::preUpdate()
    {
        AikoInput::it().pollEvents();
    }

    void InputModule::postUpdate()
    {
        AikoInput::it().clearEvents();
    }
}
