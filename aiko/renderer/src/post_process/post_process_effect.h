#pragma once

#include <string>
#include <utility>

#include <aiko_types.h>

namespace aiko
{
    class AikoRenderer;

    enum class PostProcessDomain
    {
        HDR,
        LDR
    };

    class PostProcessEffect
    {
    public:
        explicit PostProcessEffect(std::string name, bool builtIn)
            : m_name(std::move(name))
            , m_builtIn(builtIn)
        {
            AIKO_ASSERT(m_name.empty() == false, "Post-process effect name cannot be empty");
        }

        virtual ~PostProcessEffect() = default;

        const std::string& name() const
        {
            return m_name;
        }

        bool enabled() const
        {
            return m_enabled;
        }

        void setEnabled(bool enabled)
        {
            m_enabled = enabled;
        }

        bool builtIn() const
        {
            return m_builtIn;
        }

        virtual PostProcessDomain inputDomain() const = 0;
        virtual PostProcessDomain outputDomain() const = 0;

        virtual void resize(u32 width, u32 height)
        {
            AIKO_UNUSED(width);
            AIKO_UNUSED(height);
        }

    private:
        std::string m_name;
        bool m_enabled = true;
        bool m_builtIn = false;
    };
}
