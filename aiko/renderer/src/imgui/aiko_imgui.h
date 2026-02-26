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
        void init(ViewId, void*);
        void beginFrame();
        void endFrame(int width, int height);
    private:
        ViewId m_viewId;
        bool m_isInitialized;
    };
}
