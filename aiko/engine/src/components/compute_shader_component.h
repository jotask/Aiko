#pragma once

#include "models/component.h"

namespace aiko
{

    class ComputeShaderComponent : public Component
    {
    public:
        ComputeShaderComponent();
        virtual ~ComputeShaderComponent() = default;

        void load(string path);
        void setShaderId(const AssetId& id) { m_shaderId = id; }
        const AssetId& getShaderId() const { return m_shaderId; }

    private:

        AssetId m_shaderId = InvalidAssetId;

    };

}
