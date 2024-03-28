#pragma once

#include <core/libs.h>

#include "base_module.h"
#include "models/camera.h"

namespace aiko
{
    
    class AssetModule : public BaseModule
    {
    public:
    
        AssetModule() = default;
        virtual ~AssetModule() = default;

    protected:
    
        virtual void init() override;

        void loadMesh(const char* name);

        Camera m_camera;
    
    };

}
