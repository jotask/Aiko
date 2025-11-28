#pragma once

#include "assets/asset.h"
#include "models/shader.h"

namespace aiko
{
    namespace asset
    {
        class ShaderAsset : public Asset
        {
        protected:
            virtual void connect(AssetConnector*) override;
        private:
            Shader m_shader;
        };

    }
}
