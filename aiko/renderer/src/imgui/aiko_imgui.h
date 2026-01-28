#pragma once

class GLFWwindow;

namespace aiko
{
    class AikoImgui
    {
        friend class AikoRenderer;
        static void init(GLFWwindow* window);
        static void beginFrame();
        static void endFrame();
    };
}
