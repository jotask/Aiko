#pragma once

#include "base_module.h"

#include "input/aiko_input.h"

namespace aiko
{
    class DisplayModule;
    class AikoInput;
    class InputModule : public BaseModule
    {
    public:

        InputModule(Aiko* aiko) : BaseModule(aiko) { };
        virtual ~InputModule() override = default;

        virtual void connect(ModuleConnector*) override;
        virtual void init() override;
        virtual void preUpdate() override;
        virtual void postUpdate() override;

        AikoInput& input()
        {
            return m_input;
        }

        const AikoInput& input() const
        {
            return m_input;
        }

    private:
        DisplayModule* m_displayModule = nullptr;
        AikoInput m_input;

    };

}
