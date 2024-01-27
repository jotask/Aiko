#pragma once

#include "aiko_types.h"
#include "aiko.h"

namespace aiko
{

    class Application
    {
    public:

        friend class Aiko;

        Application();
        virtual ~Application() = default;

    protected:
        const aiko::AikoUPtr<Aiko> m_aiko;

        virtual void init() { initEditor(); }
        virtual void update() {}
        virtual void render() {}

    private:

        void initEditor();


    };

}
