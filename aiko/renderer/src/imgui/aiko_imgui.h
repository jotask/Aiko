#pragma once

#include "types/render_types.h"

class GLFWwindow;

namespace aiko
{
    class AikoImgui
    {
    public:
        AikoImgui();
        ~AikoImgui() = default;
        void init(const ViewId id, GLFWwindow*);
        void beginFrame(int width, int height);
        void endFrame(int width, int height);
    private:
        GLFWwindow* m_window;
        ViewId m_viewId;
        bool m_isInitialized;
    };
}
