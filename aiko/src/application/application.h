#pragma once

#include "aiko_types.h"
#include "aiko.h"

namespace aiko
{

    class Camera;

    class Application
    {
    public:

        friend class Aiko;

        Application();
        virtual ~Application() = default;

        void run();

    protected:

        virtual void init() {};
        virtual void update() {}
        virtual void render() {}

        vec2 getDisplaySize() const;
        RenderTexture2D* getTargetTexture() const;

    protected:
        aiko::AikoUPtr<Aiko> m_aiko;

        Camera* getMainCamera();

    private:


    };

}
