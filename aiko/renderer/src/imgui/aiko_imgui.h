#pragma once

#include "types/render_types.h"

class GLFWwindow;

namespace aiko
{

    class AikoImguiImpl
    {
        public:
        virtual ~AikoImguiImpl() = default;
        virtual void init(const ViewId id, GLFWwindow*) = 0;
        virtual void beginFrame(const ViewId viewId, int width, int height) = 0;
        virtual void endFrame(const ViewId viewId, int width, int height) = 0;
        virtual void dispose() = 0;
    };

    class AikoImgui
    {
    public:
        AikoImgui();
        ~AikoImgui() = default;
        void init(GLFWwindow*);
        void beginFrame(int width, int height);
        void endFrame(int width, int height);
        void dispose();
    private:
        GLFWwindow* m_window;
        const ViewId m_viewId = IMGUI_VIEW;
        bool m_isInitialized;
        AikoPtr<AikoImguiImpl> backend;
    };
}
