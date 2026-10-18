#include "asset_registry_serializer.h"

#include "asset_registry.h"
#include <logger/logger.h>

#include <algorithm>
#include <fstream>
#include <string_view>
#include <system_error>

#include <nlohmann/json.hpp>

namespace aiko
{

    namespace
    {

        using Json = nlohmann::ordered_json;

        constexpr u32 ASSET_REGISTRY_VERSION = 2;

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


        bool isValidUtf8(string_view text)
        {
            size_t i = 0;

            while (i < text.size())
            {
                const auto byte = [&text](size_t index)
                {
                    return static_cast<unsigned char>(text[index]);
                };

                const unsigned char first = byte(i);

                if (first < 0x80)
                {
                    ++i;
                    continue;
                }

                size_t length = 0;

                if (first >= 0xC2 && first <= 0xDF)
                {
                    length = 2;
                }
                else if (first >= 0xE0 && first <= 0xEF)
                {
                    length = 3;
                }
                else if (first >= 0xF0 && first <= 0xF4)
                {
                    length = 4;
                }
                else
                {
                    return false;
                }

                if (length > text.size() - i)
                {
                    return false;
                }

                const unsigned char second = byte(i + 1);

                if ((second & 0xC0) != 0x80)
                {
                    return false;
                }

                // Reject overlong encodings, UTF-16 surrogate code
                // points, and Unicode values beyond U+10FFFF.
                if ((first == 0xE0 && second < 0xA0) ||
                    (first == 0xED && second > 0x9F) ||
                    (first == 0xF0 && second < 0x90) ||
                    (first == 0xF4 && second > 0x8F))
                {
                    return false;
                }

                for (size_t j = 2; j < length; ++j)
                {
                    if ((byte(i + j) & 0xC0) != 0x80)
                    {
                        return false;
                    }
                }

                i += length;
            }

            return true;
        }


    }

    bool AssetRegistrySerializer::save(const AssetRegistry& registry, const std::filesystem::path& path)
    {
        Json root;
        root["version"] = ASSET_REGISTRY_VERSION;
        root["assets"] = Json::array();

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

                if (lhs->textureColorSpace != rhs->textureColorSpace)
                {
                    return static_cast<int>(lhs->textureColorSpace) < static_cast<int>(rhs->textureColorSpace);
                }

                return lhs->id.get() < rhs->id.get();
            });

        for (const AssetRecord* record : records)
        {

            if (!isValidUtf8(record->id.get()) || !isValidUtf8(record->source))
            {
                logger::Log::errorFmt("Cannot save asset registry: invalid UTF-8 in asset ID or source");
                return false;
            }

            Json entry = {
                { "id", record->id.get() },
                { "type", assetTypeToString(record->type) },
                { "source", record->source }
            };

            if (record->type == AssetType::Texture)
            {
                entry["colorSpace"] = record->textureColorSpace == TextureColorSpace::SRGB ? "SRGB" : "Linear";
            }

            root["assets"].push_back(std::move(entry));
        }

        if (path.has_parent_path())
        {
            std::error_code error;
            std::filesystem::create_directories(path.parent_path(), error);

            if (error)
            {
                logger::Log::errorFmt("Cannot create asset registry directory: {}", error.message());
                return false;
            }
        }

        std::ofstream file(path, std::ios::out | std::ios::trunc);

        if (!file.is_open())
        {
            return false;
        }

        file << root.dump(4) << '\n';
        file.flush();

        const bool written = file.good();

        file.close();

        return written && !file.fail();
    }

    bool AssetRegistrySerializer::load(AssetRegistry& registry, const std::filesystem::path& path)
    {
        const auto fail = [&](const char* reason) -> bool
        {
            logger::Log::errorFmt("Failed to load asset registry '{}': {}", path.string(), reason);
            return false;
        };

        std::ifstream file(path, std::ios::in | std::ios::binary);

        if (!file.is_open())
        {
            return fail("Could not open file");
        }

        Json root = Json::parse(file, nullptr, false);

        if (file.bad())
        {
            return fail("Could not read file");
        }

        if (root.is_discarded())
        {
            return fail("Invalid JSON");
        }

        if (!root.is_object())
        {
            return fail("Registry root must be an object");
        }

        const auto versionIt = root.find("version");
        const auto assetsIt = root.find("assets");

        if (versionIt == root.end() || !versionIt->is_number_integer() || (*versionIt != 1 && *versionIt != ASSET_REGISTRY_VERSION))
        {
            return fail("Unsupported or invalid registry version");
        }

        const int registryVersion = versionIt->get<int>();

        if (assetsIt == root.end() || !assetsIt->is_array())
        {
            return fail("Registry assets must be an array");
        }

        AssetRegistry loadedRegistry;

        for (const Json& assetJson : *assetsIt)
        {
            if (!assetJson.is_object())
            {
                return fail("Asset entry must be an object");
            }

            const auto idIt = assetJson.find("id");
            const auto typeIt = assetJson.find("type");
            const auto sourceIt = assetJson.find("source");

            if (idIt == assetJson.end() || typeIt == assetJson.end() || sourceIt == assetJson.end())
            {
                return fail("Asset entry has missing fields");
            }

            if (!idIt->is_string() || !typeIt->is_string() || !sourceIt->is_string())
            {
                return fail("Asset fields must be strings");
            }

            const string& id = idIt->get_ref<const string&>();

            const string& typeName = typeIt->get_ref<const string&>();

            const string& source = sourceIt->get_ref<const string&>();

            if (id.empty() || source.empty())
            {
                return fail("Asset ID and source cannot be empty");
            }

            const AssetId assetId(id);

            if (assetId == InvalidAssetId)
            {
                return fail("Asset ID cannot be invalid");
            }

            const AssetType type = assetTypeFromString(typeName);

            if (type == AssetType::Unknown)
            {
                return fail("Unknown asset type");
            }

            TextureColorSpace colorSpace = TextureColorSpace::Linear;

            const auto colorSpaceIt = assetJson.find("colorSpace");

            if (type == AssetType::Texture && registryVersion == 2)
            {
                if (colorSpaceIt == assetJson.end() || !colorSpaceIt->is_string())
                {
                    return fail("Texture has missing or invalid color space");
                }

                const string value = colorSpaceIt->get<string>();

                if (value == "Linear")
                {
                    colorSpace = TextureColorSpace::Linear;
                }
                else if (value == "SRGB")
                {
                    colorSpace = TextureColorSpace::SRGB;
                }
                else
                {
                    return fail("Unknown texture color space");
                }
            }
            else if (colorSpaceIt != assetJson.end())
            {
                return fail("Unexpected asset color space");
            }

            // Validate uniqueness before calling registerAsset().
            // registerAsset() asserts if its invariants are violated.
            if (loadedRegistry.find(assetId) != nullptr)
            {
                return fail("Duplicate asset ID");
            }

            if (loadedRegistry.find(type, source, colorSpace) != nullptr)
            {
                return fail("Duplicate asset type/source/color space");
            }

            const AssetRecord record
            {
                .id = assetId,
                .type = type,
                .source = source,
                .textureColorSpace = colorSpace
            };

            loadedRegistry.registerAsset(record);
        }

        // Commit only after every entry has been validated.
        registry = std::move(loadedRegistry);

        return true;
    }

}
