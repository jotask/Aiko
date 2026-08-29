#pragma once

#include <unordered_map>

#include "assets/asset_record.h"

namespace aiko
{

    class AssetRegistry
    {
    public:

        AssetId registerAsset(AssetType type, string_view source);

        const AssetRecord* find(const AssetId& id) const;
        const AssetRecord* find(AssetType type, string_view source) const;

        bool contains(const AssetId& id) const;

        void clear();

    private:

        std::unordered_map<AssetId, AssetRecord> m_records;
        std::unordered_map<AssetType, std::unordered_map<string, AssetId>> m_sourceIndex;

    };

}
