#pragma once

#include <core/libs.h>

#include "base_module.h"

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
    
    };

}
