#include "modules/input_module.h"

#include "module_connector.h"
#include "modules/display_module.h"

#include <input/aiko_input.h>

namespace aiko
{
    void InputModule::connect(ModuleConnector* moduleConnector)
    {
        BIND_MODULE_REQUIRED(DisplayModule, moduleConnector, m_displayModule);
    }

    void InputModule::init()
    {
        GLFWwindow* native = static_cast<GLFWwindow*>(m_displayModule->getNativeWindow());
        AikoInput::it().init(native);
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
