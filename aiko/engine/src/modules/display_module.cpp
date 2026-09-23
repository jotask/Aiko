#include "display_module.h"

#include "aiko.h"

#include <display/display_manager.h>

namespace aiko
{
    DisplayModule::DisplayModule(Aiko* aiko)
        : BaseModule(aiko)
    {
    }

    ivec2 DisplayModule::getDisplaySize() const
    {
        return m_manager.getDisplay()->getDisplaySize();
    }

    ivec2 DisplayModule::getWindowSize() const
    {
        return m_manager.getWindowSize();
    }

    ivec2 DisplayModule::getFramebufferSize() const
    {
        return m_manager.getFramebufferSize();
    }

    vec2 DisplayModule::windowToFramebuffer(const vec2& position) const
    {
        const ivec2 windowSize = getWindowSize();
        const ivec2 framebufferSize = getFramebufferSize();

        if (windowSize.x <= 0 ||
            windowSize.y <= 0 ||
            framebufferSize.x <= 0 ||
            framebufferSize.y <= 0)
        {
            return {};
        }

        const vec2 scale =
        {
            static_cast<float>(framebufferSize.x) / static_cast<float>(windowSize.x),
            static_cast<float>(framebufferSize.y) / static_cast<float>(windowSize.y)
        };

        return position * scale;
    }

    void* DisplayModule::getNativeWindow() const
    {
        return m_manager.getNativeWindow();
    }

    void DisplayModule::preInit()
    {
        const AikoConfig& cfg = getAiko()->getConfig();
        m_manager.init(cfg.windowTitle, cfg.width, cfg.height);
    }

    void DisplayModule::preUpdate()
    {
        m_manager.update();
    }

    void DisplayModule::endFrame()
    {
        m_manager.swap();
    }

}
