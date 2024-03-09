#pragma once

#include "aiko_types.h"
#include "aiko.h"
#include "types/textures.h"
#include "types/inputs.h"
#include "shared/math.h"

namespace aiko
{

    namespace asset
    {
        class Shader;
    }

    class Camera;
    class RenderSystem;
    class InputSystem;
    class AssetSystem;

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

        AssetSystem* getAssetSystem() const;
        InputSystem* getInputSystem() const;
        RenderSystem* getRenderSystem() const;

        Camera* getMainCamera();
        vec2 getViewportSize() const;
        float getAspectRatio() const;
        texture::RenderTexture2D* getTargetTexture() const;

        aiko::AikoUPtr<Aiko> m_aiko;

        aiko::asset::Shader* loadShader(const char* vs, const char* fs);
        void unloadShader(aiko::asset::Shader*);

    private:

    };

}
