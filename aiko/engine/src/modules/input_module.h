#pragma once

#include "base_module.h"

namespace aiko
{
    class DisplayModule;
    class InputModule : public BaseModule
    {
    public:

        InputModule(Aiko* aiko) : BaseModule(aiko) { };
        virtual ~InputModule() override = default;

        virtual void connect(ModuleConnector*) override;
        virtual void init() override;
        virtual void preUpdate() override;
        virtual void postUpdate() override;

    private:
        DisplayModule* m_displayModule = nullptr;

    };

}
