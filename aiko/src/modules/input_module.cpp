#include "modules/input_module.h"

#include "models/time.h"

#include "aiko_types.h"
#include "core/libs.h"
#include "modules/module_connector.h"
#include "modules/display_module.h"

#include <GLFW/glfw3.h>

namespace aiko
{

    bool InputModule::isKeyPressed(Key key) const
    {
        return false;
    }

    vec2 InputModule::getMousePosition() const
    {
        return {};
    }

    bool InputModule::isMouseButtonPressed(MouseButton button) const
    {
        return false;
    }


    void InputModule::connect(ModuleConnector* moduleConnector)
    {
        BIND_MODULE_REQUIRED(DisplayModule, moduleConnector, m_displayModule)
    }

    void InputModule::init()
    {

    }

    void InputModule::preUpdate()
    {
        GLFWwindow* window = (GLFWwindow*)m_displayModule->getNativeDisplay();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
    }

}
