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
        virtual void beginFrame(int width, int height) = 0;
        virtual void endFrame(int width, int height) = 0;
    };

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
        AikoPtr<AikoImguiImpl> backend;
    };
}
