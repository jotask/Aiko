#include "cartridge.h"

#include <fstream>

#include "aiko_includes.h"

#include "nes_types.h"
#include "memory.h"
#include "bus.h"

namespace nes
{

    void Cartridge::load(const char* file)
    {

        constexpr size_t skipBytes = 0x0010; // Header NES data
        constexpr size_t loadBytes = 0x4000; // Number of bytes to load from the file

        std::ifstream stream;
        stream.open(file, std::ios::binary);
        if (stream.is_open() == false)
        {
            aiko::Log::error("Couldn't load binary file: ", file);
            return;
        }

        // Get the size of the file
        stream.seekg(0, std::ios::end);
        std::streampos fileSize = stream.tellg();
        stream.seekg(0, std::ios::beg);

        std::vector<char> buffer(fileSize);
        stream.read(buffer.data(), fileSize);
        if (!stream)
        {
            aiko::Log::error("Error reading binary file: ", file);
            return;
        }

        buffer.resize(buffer.size() - loadBytes);
        buffer.erase(buffer.begin(), buffer.begin() + skipBytes);

        stream.close();

        Memory* memory = bus->getMicroprocesor<Memory>();
        assert(memory != nullptr, "Couldn't get Memory connected to the bus");

        std::copy(buffer.begin(), buffer.end(), memory->data + 0x8000);
        std::copy(buffer.begin(), buffer.end(), memory->data + 0xC000);

    }

    void Cartridge::load(std::vector<Byte>& memoryDump)
    {
        assert(memoryDump.size() < nes::Memory::MAX_MEM, "The Memory dump passed is bigger than supported size");
        memoryDump.resize(nes::Memory::MAX_MEM);
        Memory* memory = bus->getMicroprocesor<Memory>();
        assert(memory != nullptr, "Couldnt' get Memory connected to the bus");
        assert(memoryDump.size() == Memory::MAX_MEM, "Couldnt' get Memory connected to the bus");
        std::copy(memoryDump.begin(), memoryDump.end(), memory->data);
    }

    void Cartridge::reset()
    {

    }

}
