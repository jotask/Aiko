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

        ivec2 getDisplaySize() const;
        void* getNativeWindow() const;

    protected:

        virtual void preInit() override;
        virtual void preUpdate() override;
        virtual void endFrame() override;

    private:
        DisplayManager m_manager;

    };

}
