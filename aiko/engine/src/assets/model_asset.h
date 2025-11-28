#pragma once

#include "assets/asset.h"
#include "models/model.h"

namespace aiko
{
    namespace asset
    {
        class ModelAsset : public Asset
        {
        public:
        protected:
            virtual void connect(AssetConnector*) override;
        private:
            Model m_model;
        };

    }
}
