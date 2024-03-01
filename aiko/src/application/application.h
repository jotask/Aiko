#pragma once

#include "aiko_types.h"
#include "aiko.h"
#include "core/textures.h"

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

        ivec2 getDisplaySize() const;
        texture::RenderTexture2D* getTargetTexture() const;

    protected:
        aiko::AikoUPtr<Aiko> m_aiko;

        Camera* getMainCamera();

    private:


    };

}
