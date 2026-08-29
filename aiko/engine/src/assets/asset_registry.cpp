#include "asset_registry.h"

namespace aiko
{

    AssetId AssetRegistry::registerAsset(AssetType type, string_view source)
    {
        AIKO_ASSERT(type != AssetType::Unknown, "Cannot register asset with unknown type");
        AIKO_ASSERT(source.empty() == false, "Cannot register asset with empty source");

        const AssetRecord* existing = find(type, source);
        if (existing != nullptr)
        {
            return existing->id;
        }

        AssetRecord record
        {
            .id = AssetId{},
            .type = type,
            .source = string(source)
        };

        const AssetId id = record.id;

        auto [recordIt, recordInserted] = m_records.emplace(id, std::move(record));
        AIKO_ASSERT(recordInserted, "Asset id already registered");

        auto& typeIndex = m_sourceIndex[type];
        auto [sourceIt, sourceInserted] = typeIndex.emplace(recordIt->second.source, id);
        AIKO_UNUSED(sourceIt);

        AIKO_ASSERT(sourceInserted, "Asset source already registered");

        return id;
    }

    const AssetRecord* AssetRegistry::find(const AssetId& id) const
    {
        const auto it = m_records.find(id);
        if (it == m_records.end())
        {
            return nullptr;
        }

        return &it->second;
    }

    const AssetRecord* AssetRegistry::find(AssetType type, string_view source) const
    {
        const auto typeIt = m_sourceIndex.find(type);
        if (typeIt == m_sourceIndex.end())
        {
            return nullptr;
        }

        const auto sourceIt = typeIt->second.find(string(source));
        if (sourceIt == typeIt->second.end())
        {
            return nullptr;
        }

        return find(sourceIt->second);
    }

    bool AssetRegistry::contains(const AssetId& id) const
    {
        return m_records.find(id) != m_records.end();
    }

    void AssetRegistry::clear()
    {
        m_sourceIndex.clear();
        m_records.clear();
    }

}
