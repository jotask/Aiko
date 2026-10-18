#pragma once

#include <unordered_map>

#include "assets/asset_record.h"

namespace aiko
{

    class AssetRegistrySerializer;

    class AssetRegistry
    {
    public:

        AssetId registerAsset(AssetType type, string_view source, TextureColorSpace colorSpace = TextureColorSpace::Linear);
        AssetId registerAsset(const AssetRecord& record);

        const AssetRecord* find(const AssetId& id) const;
        const AssetRecord* find(AssetType type, string_view source, TextureColorSpace colorSpace = TextureColorSpace::Linear) const;

        bool contains(const AssetId& id) const;

        void clear();

    private:
        friend class AssetRegistrySerializer;

        std::unordered_map<AssetId, AssetRecord> m_records;
        std::unordered_map<AssetType, std::unordered_map<string, std::unordered_map<TextureColorSpace, AssetId>>> m_sourceIndex;

    };

}
