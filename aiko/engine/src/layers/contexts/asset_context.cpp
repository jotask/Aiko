#include "asset_context.h"

#include <array>
#include <filesystem>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include <constants.h>
#include <core/file.h>

#include "models/font.h"
#include "assets/types/texture_asset.h"
#include "systems/system_connector.h"
#include "systems/asset_system.h"

namespace aiko
{

    AssetContext::AssetContext(SystemConnector& connector)
    {
        m_assetSystem = connector.find<AssetSystem>();
        AIKO_ASSERT(m_assetSystem != nullptr, "Required system AssetSystem not found");
    }

    AssetId AssetContext::loadShader(string_view source)
    {
        const AssetId id = m_assetSystem->registerAsset<ShaderAsset>(source);
        m_assetSystem->loadAsset<ShaderAsset>(id);
        return id;
    }

    AssetId AssetContext::loadTexture(string_view source)
    {
        const AssetId id = m_assetSystem->registerAsset<TextureAsset>(source);
        m_assetSystem->loadAsset<TextureAsset>(id);
        return id;
    }

    Font AssetContext::loadFont(string_view source, float pixelSize)
    {
        AIKO_ASSERT(source.empty() == false, "Font source cannot be empty");
        AIKO_ASSERT(pixelSize > 0.0f, "Font pixel size must be greater than zero");

        const std::filesystem::path path = std::filesystem::path(global::GLOBAL_ASSET_PATH) / source;
        const vector<uint8_t> fontData = files::readFileBytes(path);
        AIKO_ASSERT(fontData.empty() == false, "Font file is empty");

        constexpr int AtlasWidth = 512;
        constexpr int AtlasHeight = 512;

        std::vector<unsigned char> atlas(AtlasWidth * AtlasHeight, 0);

        std::array<stbtt_packedchar, Font::CharacterCount> packedCharacters{};

        stbtt_pack_context packContext{};

        const int beginResult = stbtt_PackBegin(&packContext, atlas.data(), AtlasWidth, AtlasHeight, 0, 1, nullptr);
        AIKO_ASSERT(beginResult != 0, "Failed to initialize font atlas");

        const int packResult = stbtt_PackFontRange(&packContext, fontData.data(), 0, pixelSize, Font::FirstCharacter, Font::CharacterCount, packedCharacters.data());
        stbtt_PackEnd(&packContext);
        AIKO_ASSERT(packResult != 0, "Failed to pack font glyphs");

        stbtt_fontinfo fontInfo{};

        const int fontOffset = stbtt_GetFontOffsetForIndex(fontData.data(), 0);
        AIKO_ASSERT(fontOffset >= 0, "Failed to resolve font offset");

        const int initResult = stbtt_InitFont(&fontInfo, fontData.data(), fontOffset);
        AIKO_ASSERT(initResult != 0, "Failed to initialize font");

        const float metricScale = stbtt_ScaleForPixelHeight(&fontInfo, pixelSize);

        int ascent = 0;
        int descent = 0;
        int lineGap = 0;

        stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);

        TextureAsset atlasAsset{};
        atlasAsset.desc.type = TextureType::Sampled;
        atlasAsset.desc.format = TextureFormat::RGBA8;
        atlasAsset.desc.width = AtlasWidth;
        atlasAsset.desc.height = AtlasHeight;
        atlasAsset.desc.mipmaps = 1;
        atlasAsset.desc.computeWrite = false;

        atlasAsset.pixels.resize(AtlasWidth * AtlasHeight);

        for (int y = 0; y < AtlasHeight; ++y)
        {
            for (int x = 0; x < AtlasWidth; ++x)
            {
                const int sourceY = AtlasHeight - 1 - y;
                const unsigned char alpha = atlas[sourceY * AtlasWidth + x];
                atlasAsset.pixels[y * AtlasWidth + x] = Color::fromBytes(255, 255, 255, alpha);
            }
        }

        Font font;
        font.m_atlasTexture = m_assetSystem->create(atlasAsset);
        font.m_pixelSize = pixelSize;
        font.m_ascent = static_cast<float>(ascent) * metricScale;
        font.m_descent = static_cast<float>(descent) * metricScale;
        font.m_lineHeight = static_cast<float>(ascent - descent + lineGap) * metricScale;

        for (int i = 0; i < Font::CharacterCount; ++i)
        {
            const stbtt_packedchar& packed = packedCharacters[static_cast<size_t>(i)];

            FontGlyph& glyph = font.m_glyphs[static_cast<size_t>(i)];

            glyph.uvMin =
            {
                static_cast<float>(packed.x0) / AtlasWidth,
                1.0f - static_cast<float>(packed.y0) / AtlasHeight
            };

            glyph.uvMax =
            {
                static_cast<float>(packed.x1) / AtlasWidth,
                1.0f - static_cast<float>(packed.y1) / AtlasHeight
            };

            glyph.offset =
            {
                packed.xoff,
                packed.yoff
            };

            glyph.size =
            {
                packed.xoff2 - packed.xoff,
                packed.yoff2 - packed.yoff
            };

            glyph.advance = packed.xadvance;
        }

        return font;
    }

}
