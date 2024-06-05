#pragma once

#include "assets/asset.h"
#include "types/textures.h"

namespace aiko
{
    namespace asset
    {
        class TextureAsset : public Asset
        {
        public:
        protected:
            virtual void connect(AssetConnector*) override;
        private:
            texture::Texture m_texture;
        };

    }
}
