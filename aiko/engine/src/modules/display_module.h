#pragma once

#include "base_module.h"

#include <display/display_manager.h>

namespace aiko
{

    class DisplayModule : public BaseModule
    {
    
    public:

        DisplayModule(Aiko* aiko);
        virtual ~DisplayModule() = default;

        const ivec2 getDisplaySize() const;

    protected:

        virtual void preInit() override;
        virtual void preUpdate() override;
        virtual void endFrame() override;


    private:
        // AikoUPtr<DisplayManager> m_manager;

    };

}
