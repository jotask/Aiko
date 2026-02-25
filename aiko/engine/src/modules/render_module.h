#pragma once

#include "modules/base_module.h"

namespace aiko
{

    class Camera;
    class DisplayModule;
    class Mesh;
    class Shader;
    class Transform;

    class RenderModule : public BaseModule
    {
    
    public:

        RenderModule(Aiko* aiko);
        virtual ~RenderModule() = default;

        void setMainCamera(const Camera* camera); // TEMPORAL, this should be removed from here

    protected:

        virtual void init() override;
        virtual void beginFrame() override;
        virtual void endFrame() override;
        virtual void dispose() override;

    private:

        const Camera* m_mainCamera;

    };

}
