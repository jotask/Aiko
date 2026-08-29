#include "asset_registry_serializer.h"

#include "asset_registry.h"

#include <algorithm>
#include <fstream>

#include <nlohmann/json.hpp>

namespace aiko
{

    namespace
    {

        constexpr u32 ASSET_REGISTRY_VERSION = 1;

        const char* assetTypeToString(AssetType type)
        {
            switch (type)
            {
                case AssetType::Texture:
                    return "Texture";

                case AssetType::Mesh:
                    return "Mesh";

                case AssetType::Model:
                    return "Model";

                case AssetType::Shader:
                    return "Shader";

                case AssetType::ComputeShader:
                    return "ComputeShader";

                case AssetType::Material:
                    return "Material";

                case AssetType::Unknown:
                default:
                    return "Unknown";
            }
        }

        AssetType assetTypeFromString(string_view type)
        {
            if (type == "Texture")
            {
                return AssetType::Texture;
            }

            if (type == "Mesh")
            {
                return AssetType::Mesh;
            }

            if (type == "Model")
            {
                return AssetType::Model;
            }

            if (type == "Shader")
            {
                return AssetType::Shader;
            }

            if (type == "ComputeShader")
            {
                return AssetType::ComputeShader;
            }

            if (type == "Material")
            {
                return AssetType::Material;
            }

            return AssetType::Unknown;
        }

    }

    bool AssetRegistrySerializer::save(const AssetRegistry& registry, const std::filesystem::path& path)
    {
        nlohmann::json root;
        root["version"] = ASSET_REGISTRY_VERSION;
        root["assets"] = nlohmann::json::array();

        vector<const AssetRecord*> records;
        records.reserve(registry.m_records.size());

        for (const auto& [id, record] : registry.m_records)
        {
            AIKO_UNUSED(id);
            records.push_back(&record);
        }

        std::sort(
            records.begin(),
            records.end(),
            [](const AssetRecord* lhs, const AssetRecord* rhs)
            {
                if (lhs->type != rhs->type)
                {
                    return static_cast<u8>(lhs->type) < static_cast<u8>(rhs->type);
                }

                if (lhs->source != rhs->source)
                {
                    return lhs->source < rhs->source;
                }

                return lhs->id.get() < rhs->id.get();
            });

        for (const AssetRecord* record : records)
        {
            root["assets"].push_back(
            {
                { "id", record->id.get() },
                { "type", assetTypeToString(record->type) },
                { "source", record->source }
            });
        }

        if (path.has_parent_path())
        {
            std::filesystem::create_directories(path.parent_path());
        }

        std::ofstream file(path);
        if (file.is_open() == false)
        {
            return false;
        }

        file << root.dump(4) << '\n';

        return file.good();
    }

    bool AssetRegistrySerializer::load(AssetRegistry& registry, const std::filesystem::path& path)
    {
        if (std::filesystem::exists(path) == false)
        {
            return false;
        }

        std::ifstream file(path);
        if (file.is_open() == false)
        {
            return false;
        }

        try
        {
            nlohmann::json root;
            file >> root;

            if (root.contains("version") == false || root.contains("assets") == false)
            {
                return false;
            }

            if (root["version"].get<u32>() != ASSET_REGISTRY_VERSION)
            {
                return false;
            }

            if (root["assets"].is_array() == false)
            {
                return false;
            }

            AssetRegistry loadedRegistry;

            for (const nlohmann::json& assetJson : root["assets"])
            {
                if (assetJson.contains("id") == false || assetJson.contains("type") == false || assetJson.contains("source") == false)
                {
                    return false;
                }

                const string id = assetJson["id"].get<string>();
                const string typeName = assetJson["type"].get<string>();
                const string source = assetJson["source"].get<string>();

                const AssetType type = assetTypeFromString(typeName);
                if (type == AssetType::Unknown)
                {
                    return false;
                }

                AssetRecord record
                {
                    .id = AssetId(id),
                    .type = type,
                    .source = source
                };

                loadedRegistry.registerAsset(record);
            }

            registry = std::move(loadedRegistry);

            return true;
        }
        catch (const std::exception&)
        {
            return false;
        }
    }

}
