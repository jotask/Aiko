#pragma once

#include "base_module.h"

namespace aiko
{
    
    class AssetModule : public BaseModule
    {
    public:

        AssetModule(Aiko* aiko) : BaseModule(aiko) { };
        virtual ~AssetModule() = default;

    protected:
    
        virtual void init() override;

        void loadMesh(const char* name);
    
    };

}
