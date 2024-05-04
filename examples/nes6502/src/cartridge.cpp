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
        std::ifstream stream;
        stream.open(file, std::ios::binary);
        if (stream.is_open() == false)
        {
            aiko::Log::error("Couldn't load binary file: ", file);
            return;
        }

        Memory* memory = bus->getMicroprocesor<Memory>();

        // Get the size of the file
        stream.seekg(0, std::ios::end);
        std::streampos fileSize = stream.tellg();
        stream.seekg(0, std::ios::beg);

        assert(Memory::MAX_MEM == fileSize, "Oh dear, this doesn't match");

        // Read the file into the memory
        stream.read(reinterpret_cast<char*>(memory->data), fileSize);

        if (!stream)
        {
            aiko::Log::error("Error reading binary file: ", file);
            return;
        }

        stream.close();

    }

    void Cartridge::load(std::vector<Byte>& memoryDump)
    {
        Memory* memory = bus->getMicroprocesor<Memory>();
        assert(memory != nullptr, "Couldnt' get Memory connected to the bus");
        assert(memoryDump.size() == Memory::MAX_MEM, "Couldnt' get Memory connected to the bus");
        std::copy(memoryDump.begin(), memoryDump.end(), memory->data);
    }

    void Cartridge::reset()
    {

    }

}
