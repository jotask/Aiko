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

        return registerAsset(record);
    }

    AssetId AssetRegistry::registerAsset(const AssetRecord& record)
    {
        AIKO_ASSERT(record.id != InvalidAssetId, "Cannot register asset with invalid id");
        AIKO_ASSERT(record.type != AssetType::Unknown, "Cannot register asset with unknown type");
        AIKO_ASSERT(record.source.empty() == false, "Cannot register asset with empty source");

        const AssetRecord* existingById = find(record.id);
        if (existingById != nullptr)
        {
            AIKO_ASSERT(existingById->type == record.type, "Asset id already registered with different type");
            AIKO_ASSERT(existingById->source == record.source, "Asset id already registered with different source");
            return existingById->id;
        }

        const AssetRecord* existingBySource = find(record.type, record.source);
        if (existingBySource != nullptr)
        {
            AIKO_ASSERT(existingBySource->id == record.id, "Asset source already registered with different id");
            return existingBySource->id;
        }

        auto [recordIt, recordInserted] = m_records.emplace(record.id, record);
        AIKO_ASSERT(recordInserted, "Asset id already registered");

        auto& typeIndex = m_sourceIndex[record.type];
        auto [sourceIt, sourceInserted] = typeIndex.emplace(recordIt->second.source, record.id);
        AIKO_UNUSED(sourceIt);

        AIKO_ASSERT(sourceInserted, "Asset source already registered");

        return record.id;
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
