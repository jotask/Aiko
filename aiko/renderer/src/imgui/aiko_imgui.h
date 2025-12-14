#pragma once

#include <aiko_types.h>

class GLFWwindow;

namespace aiko
{
    class AIKO_API AikoImgui
    {
        friend class AikoRenderer;
        static void init(GLFWwindow* window);
        static void beginFrame();
        static void endFrame();
    };
}
