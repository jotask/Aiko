#pragma once

#include "aiko_types.h"
#include "aiko.h"
#include "types/textures.h"
#include "types/inputs.h"
#include "shared/math.h"

namespace aiko
{

    class Camera;
    class RenderSystem;

    class Application
    {
    public:

        friend class Aiko;

        Application();
        virtual ~Application() = default;

        void run();

    protected:

        float getlDeltaTime() const;
        bool isKeyPressed(Key) const;
        vec2 getMousePosition() const;
        bool isMouseButtonPressed(MouseButton) const;

        virtual void init() {};
        virtual void update() {}
        virtual void render() {}

        RenderSystem* getRenderSystem();
        Camera* getMainCamera();
        ivec2 getDisplaySize() const;
        float getAspectRatio() const;
        texture::RenderTexture2D* getTargetTexture() const;

        aiko::AikoUPtr<Aiko> m_aiko;

    private:


    };

}
