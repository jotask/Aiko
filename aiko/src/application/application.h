#pragma once

#include "aiko_types.h"
#include "aiko.h"
#include "core/textures.h"
#include "core/inputs.h"
#include "shared/math.h"

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

        float getlDeltaTime() const;
        bool isKeyPressed(Key) const;
        vec2 getMousePosition() const;
        bool isMouseButtonPressed(MouseButton) const;

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
