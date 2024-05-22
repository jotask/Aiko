#include "cartridge.h"

#include <fstream>
#include <string>

#include "aiko_includes.h"

#include "nes_types.h"
#include "memory.h"
#include "bus.h"

namespace nes
{

    void Cartridge::load(const char* file)
    {

        constexpr size_t headerBytes = 0x0010; // Header NES data
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

        std::string constant = std::string(&buffer[0x00], &buffer[0x03]); // Constant $4E $45 $53 $1A (ASCII "NES" followed by MS-DOS end-of-file)
        char pgr_room = buffer[0x03]; // Size of PRG ROM in 16 KB units
        char chr_room = buffer[0x04]; // Size of CHR ROM in 8 KB units (value 0 means the board uses CHR RAM)
        char flags_6  = buffer[0x06]; // Flags 6 – Mapper, mirroring, battery, trainer
        char flags_7  = buffer[0x07]; // Flags 7 – Mapper, VS/Playchoice, NES 2.0
        char flags_8  = buffer[0x08]; // Flags 8 – PRG-RAM size (rarely used extension)
        char flags_9  = buffer[0x09]; // Flags 9 – TV system (rarely used extension)
        char flags_10 = buffer[0x10]; // Flags 10 – TV system, PRG-RAM presence (unofficial, rarely used extension)

        buffer.resize(buffer.size() - loadBytes);
        buffer.erase(buffer.begin(), buffer.begin() + headerBytes);

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

    bool Cartridge::cpu_read(uint16_t addr, uint8_t& data)
    {
        u32 mapped_address = 0;
        if (mapper->cpu_read( addr, mapped_address ))
        {
            data = PRG_Memory[mapped_address];
            return true;
        }
        return false;
    }

    bool Cartridge::cpu_write(uint16_t addr, uint8_t data)
    {
        u32 mapped_address = 0;
        if (mapper->cpu_write(addr, mapped_address))
        {
            PRG_Memory[mapped_address] = data;
            return true;
        }
        return false;
    }

    bool Cartridge::ppu_read(uint16_t addr, uint8_t& data)
    {
        u32 mapped_address = 0;
        if (mapper->ppu_read(addr, mapped_address))
        {
            data = CHR_Memory[mapped_address];
            return true;
        }
        return false;
    }

    bool Cartridge::ppu_write(uint16_t addr, uint8_t data)
    {
        u32 mapped_address = 0;
        if (mapper->ppu_write(addr, mapped_address))
        {
            CHR_Memory[mapped_address] = data;
            return true;
        }
        return false;
    }

}
