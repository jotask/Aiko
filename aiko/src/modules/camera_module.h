#pragma once

#include <raylib.h>

#include "base_module.h"
#include "models/camera.h"

namespace aiko
{
    
    class CameraModule : public BaseModule
    {
    public:
    
        CameraModule() = default;
        virtual ~CameraModule() = default;
    
        Camera* GetMainCamera() { return &m_camera; };

    protected:
    
        virtual void init() override;

        Camera m_camera;
    
    };

}
