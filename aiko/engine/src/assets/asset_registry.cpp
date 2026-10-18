#include "asset_registry.h"

namespace aiko
{

    AssetId AssetRegistry::registerAsset(AssetType type, string_view source, TextureColorSpace colorSpace)
    {
        AIKO_ASSERT(type != AssetType::Unknown, "Cannot register unknown asset type");
        AIKO_ASSERT(!source.empty(), "Cannot register asset with empty source");
        AIKO_ASSERT(type == AssetType::Texture || colorSpace == TextureColorSpace::Linear, "Only textures support color-space variants");
        AIKO_ASSERT(colorSpace == TextureColorSpace::Linear || colorSpace == TextureColorSpace::SRGB, "Invalid asset color space");

        if (const AssetRecord* existing = find(type, source, colorSpace))
        {
            return existing->id;
        }

        return registerAsset(AssetRecord{
            .id = AssetId{},
            .type = type,
            .source = string(source),
            .textureColorSpace = colorSpace
        });
    }

    AssetId AssetRegistry::registerAsset(const AssetRecord& record)
    {
        AIKO_ASSERT(record.id != InvalidAssetId, "Cannot register asset with invalid id");
        AIKO_ASSERT(record.type != AssetType::Unknown, "Cannot register asset with unknown type");
        AIKO_ASSERT(record.source.empty() == false, "Cannot register asset with empty source");

        AIKO_ASSERT(record.type == AssetType::Texture || record.textureColorSpace == TextureColorSpace::Linear, "Non-texture asset has a color-space variant");
        AIKO_ASSERT(record.textureColorSpace == TextureColorSpace::Linear || record.textureColorSpace == TextureColorSpace::SRGB, "Invalid asset color space");

        const AssetRecord* existingById = find(record.id);
        if (existingById != nullptr)
        {
            AIKO_ASSERT(existingById->type == record.type, "Asset id already registered with different type");
            AIKO_ASSERT(existingById->source == record.source, "Asset id already registered with different source");
            AIKO_ASSERT(existingById->textureColorSpace == record.textureColorSpace, "Asset id already registered with different color space");
            return existingById->id;
        }

        const AssetRecord* existingBySource = find(record.type, record.source, record.textureColorSpace);
        if (existingBySource != nullptr)
        {
            AIKO_ASSERT(existingBySource->id == record.id, "Asset source already registered with different id");
            return existingBySource->id;
        }

        auto [recordIt, recordInserted] = m_records.emplace(record.id, record);
        AIKO_ASSERT(recordInserted, "Asset id already registered");

        auto& variants = m_sourceIndex[record.type][recordIt->second.source];
        auto [sourceIt, sourceInserted] = variants.emplace(record.textureColorSpace, record.id);
        AIKO_UNUSED(sourceIt);
        AIKO_ASSERT(sourceInserted, "Asset source variant already registered");

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

    const AssetRecord* AssetRegistry::find(AssetType type, string_view source, TextureColorSpace colorSpace) const
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

        const auto variantIt = sourceIt->second.find(colorSpace);

        if (variantIt == sourceIt->second.end())
        {
            return nullptr;
        }

        return find(variantIt->second);
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
