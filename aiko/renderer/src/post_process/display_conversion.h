#pragma once

#include "models/material.h"

namespace aiko
{
    class PostProcessContext;

    class DisplayConversion final
    {
    public:
        explicit DisplayConversion(AssetId shaderId);

        void execute(PostProcessContext& context);

    private:
        Material m_material;
    };
}
