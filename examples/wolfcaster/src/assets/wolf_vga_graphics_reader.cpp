#include "wolf_vga_graphics_reader.h"

#include "aiko_macros.h"
#include "core/byte_utils.h"
#include "core/file.h"
#include "core/huffman_decoder.h"
#include "logger/logger.h"

#include <algorithm>

namespace wc
{
    WolfVgaGraphicsData WolfVgaGraphicsReader::load(const std::filesystem::path& path)
    {
        const std::filesystem::path VGA_HEAD_path = path / "VGAHEAD.WL1";
        AIKO_ASSERT(std::filesystem::exists(VGA_HEAD_path), "VGAHEAD file don't exist");

        const std::filesystem::path VGA_DICT_path = path / "VGADICT.WL1";
        AIKO_ASSERT(std::filesystem::exists(VGA_DICT_path), "VGADICT file don't exist");

        const std::filesystem::path VGA_GRAPH_path = path / "VGAGRAPH.WL1";
        AIKO_ASSERT(std::filesystem::exists(VGA_GRAPH_path), "VGAGRAPH file don't exist");

        const std::vector<uint8_t> vgaHeadContent = aiko::files::readFileBytes(VGA_HEAD_path);
        AIKO_ASSERT(vgaHeadContent.size() >= 6, "VGAHEAD must contain at least one chunk offset and a terminal offset");
        AIKO_ASSERT(vgaHeadContent.size() % 3 == 0, "VGAHEAD size must be divisible by 3");

        const std::vector<uint8_t> vgaDicContent = aiko::files::readFileBytes(VGA_DICT_path);
        AIKO_ASSERT(vgaDicContent.size() >= 255 * 4, "VGADICT is too small for 255 Huffman nodes");

        const std::vector<uint8_t> vgaGraphContent = aiko::files::readFileBytes(VGA_GRAPH_path);
        AIKO_ASSERT(vgaGraphContent.empty() == false, "VGAGRAPH is empty");

        AIKO_ASSERT(readU24LE(vgaHeadContent, vgaHeadContent.size() - 3) <= vgaGraphContent.size(), "VGAHEAD terminal offset exceeds VGAGRAPH size");

        std::vector<uint32_t> chunkOffsets;
        for (size_t i = 0 ; i < vgaHeadContent.size() / 3; ++i)
        {
            const uint32_t data = readU24LE(vgaHeadContent, i * 3);
            AIKO_ASSERT(data == 0xFFFFFF || data <= vgaGraphContent.size(), "VGA chunk offset exceeds VGAGRAPH bounds");
            chunkOffsets.push_back(data);
        }

        std::array<WolfVgaHuffmanNode, 255> huffmanNodes;
        for (size_t i = 0 ; i < 255; ++i)
        {
            const uint16_t byte0 = readU16LE(vgaDicContent, i * 4 + 0);
            const uint16_t byte1 = readU16LE(vgaDicContent, i * 4 + 2);
            huffmanNodes[i].bit0 = byte0;
            huffmanNodes[i].bit1 = byte1;
            AIKO_ASSERT(huffmanNodes[i].bit0 <= 510, "Invalid Huffman bit0 reference");
            AIKO_ASSERT(huffmanNodes[i].bit1 <= 510, "Invalid Huffman bit1 reference");
        }

        std::vector<uint8_t> graphBytes(vgaGraphContent);

        aiko::logger::Log::infoFmt("Wolfenstein VGA Graphics:");
        aiko::logger::Log::infoFmt("    VGAHEAD size: {} bytes", vgaHeadContent.size());
        aiko::logger::Log::infoFmt("    VGADICT size: {} bytes", vgaDicContent.size());
        aiko::logger::Log::infoFmt("    VGAGRAPH size: {} bytes", vgaGraphContent.size());

        return {
            .chunkOffsets = std::move(chunkOffsets),
            .huffmanNodes = std::move(huffmanNodes),
            .graphBytes = std::move(graphBytes),
        };
    }

    std::vector<uint8_t> WolfVgaGraphicsReader::decodeChunk(const WolfVgaGraphicsData& graphics, size_t chunkIndex)
    {
        AIKO_ASSERT(graphics.chunkOffsets.size() >= 2, "Invalid VGA graphics chunk directory");
        AIKO_ASSERT(chunkIndex < graphics.chunkOffsets.size() - 1, "VGA graphics chunk index out of bounds");

        const uint32_t chunkStart = graphics.chunkOffsets[chunkIndex];
        AIKO_ASSERT(chunkStart != 0xFFFFFF, "Cannot decode a sparse VGA graphics chunk");

        size_t nextIndex = chunkIndex + 1;
        uint32_t chunkEnd = 0;

        while (true)
        {
            AIKO_ASSERT(nextIndex < graphics.chunkOffsets.size(), "Missing VGA graphics chunk end offset");
            if (graphics.chunkOffsets[nextIndex] == 0xFFFFFF)
            {
                ++nextIndex;
                continue;
            }
            chunkEnd = graphics.chunkOffsets[nextIndex];
            AIKO_ASSERT(chunkStart < chunkEnd && chunkEnd <= graphics.graphBytes.size(), "Invalid VGA graphics chunk boundaries");
            break;
        }

        const size_t chunkLength = chunkEnd - chunkStart;
        AIKO_ASSERT(chunkLength > 4, "VGA graphics chunk has no compressed payload");

        const uint32_t expandedSize = readU32LE(graphics.graphBytes, chunkStart);
        AIKO_ASSERT(expandedSize > 0 && expandedSize <= 16 * 1024 * 1024, "Invalid VGA graphics expanded size");

        const std::span<const uint8_t> compressedData = { graphics.graphBytes.begin() + chunkStart + 4, graphics.graphBytes.begin() +chunkEnd};

        aiko::logger::Log::infoFmt(
            "VGA chunk {}: start={}, end={}, compressed={} bytes, expanded={} bytes",
            chunkIndex,
            chunkStart,
            chunkEnd,
            compressedData.size(),
            expandedSize
        );

        std::vector<uint8_t> output = HuffmanDecoder::decode(compressedData, graphics.huffmanNodes, expandedSize);
        AIKO_ASSERT(output.size() == expandedSize, "VGA graphics chunk decompression size mismatch");
        return output;

    }

    std::vector<WolfVgaPictureDimension> WolfVgaGraphicsReader::decodePictureDimensions(const WolfVgaGraphicsData& graphics, size_t pictureCount)
    {
        AIKO_ASSERT(pictureCount > 0 && pictureCount <= 4096, "Invalid VGA picture count");
        AIKO_ASSERT(graphics.chunkOffsets.size() >= 2, "Invalid VGA graphics chunk directory");

        // The implementation should extract the compressed bytes for chunk 0, Huffman-decode them to pictureCount * 4 bytes, and read the width and height pairs with your existing readU16LE() helper.

        const auto findChunkEnd = [&](size_t chunkIndex) -> uint32_t
        {
            size_t nextIndex = chunkIndex + 1;
            while (true)
            {
                AIKO_ASSERT(nextIndex < graphics.chunkOffsets.size(), "Missing VGA picture dimension chunk end offset");
                const auto data = graphics.chunkOffsets[nextIndex];
                if (data == 0xFFFFFF)
                {
                    ++nextIndex;
                    continue;
                }
                return data;
            }
        };

        const uint32_t chunkStart = graphics.chunkOffsets[0];
        const uint32_t chunkEnd = findChunkEnd(0);

        AIKO_ASSERT(chunkStart != 0xFFFFFF,"VGA picture dimension chunk is sparse");
        AIKO_ASSERT(chunkStart < chunkEnd && chunkEnd <= graphics.graphBytes.size(), "Invalid VGA picture dimension chunk boundaries");

        std::span<const uint8_t> compressedData = { graphics.graphBytes.begin() + chunkStart, graphics.graphBytes.begin() + chunkEnd };
        const auto decodedBytes = HuffmanDecoder::decode(compressedData, graphics.huffmanNodes, pictureCount * 4);
        AIKO_ASSERT(decodedBytes.size() == pictureCount * 4, "Invalid VGA picture dimension table size");

        std::vector<WolfVgaPictureDimension> dimensions;
        for (size_t i = 0 ; i < pictureCount; ++i)
        {
            uint16_t width = readU16LE(decodedBytes, i * 4);
            uint16_t height = readU16LE(decodedBytes, i * 4 + 2);
            dimensions.push_back(WolfVgaPictureDimension{
                .width = width,
                .height = height,
            });
        }

        return dimensions;
    }
}
