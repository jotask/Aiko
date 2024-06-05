#pragma once

#include "types/asset_type.h"
#include "core/uuid.h"

namespace aiko
{
    class AssetSystem;
    namespace asset
    {
        class AssetConnector;
        class Asset
        {
            friend class aiko::AssetSystem;
        public:
            const ID& getID() const { return m_id; }
        protected:
            virtual void connect(AssetConnector*) = 0;
            virtual void load() { };
            virtual void unload() { };
        private:
            const ID m_id;
        };

    }
}
