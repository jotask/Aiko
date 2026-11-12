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
        for (size_t i = 0 ; i < (vgaHeadContent.size() / 3) - 1; ++i)
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
}