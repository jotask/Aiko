#include "nes/cartridge/cartridge.h"

#include <iostream>

#include <fstream>
#include <string>
#include <cstring> // For strcmp

#include "aiko_includes.h"

#include "nes/nes_types.h"
#include "nes/memory.h"
#include "nes/bus.h"

#include "mappers/mapper_000.h"

namespace nes
{

    void Cartridge::load(const char* file)
    {

        constexpr size_t headerBytes = 0x0010; // Header NES data
        constexpr size_t loadBytes = 0x4000; // Number of bytes to load from the file

        struct CartridgeHeader
        {
            char name[4];
            Byte prg_rom_chunks;
            Byte chr_rom_chunks;
            Byte mapper1;
            Byte mapper2;
            Byte prg_ram_size;
            Byte tv_system1;
            Byte tv_system2;
            char unused[5];
        };

        CartridgeHeader header;

        std::ifstream stream;
        stream.open(file, std::ios::binary);
        if (stream.is_open() == false)
        {
            aiko::Log::error("Couldn't load binary file: ", file);
            return;
        }

        // Parse header
        stream.read((char*)&header, sizeof(CartridgeHeader));

        if (memcmp("NES\x1A", header.name, 4) != 0)
        {
            aiko::Log::error("Inserted invalid cartridge. Only iNES file are supported");
            assert(false, "UnkownFile");
            std::exit(-1);
        }

        // TODO What to do with the trainer from some roms?

        Byte mapperId = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);
        Mirroring mirror = (header.mapper1 & 0x01) ? Mirroring::Vertical : Mirroring::Horizontal;

        Byte nFileType = 1;

        assert(nFileType == 1, "TODO Not Implemented");
        
        if (nFileType == 1)
        {
            PRG_Memory.resize( header.prg_rom_chunks * 16384); // 16KiB
            stream.read((char*)PRG_Memory.data(), PRG_Memory.size());
            CHR_Memory.resize(header.chr_rom_chunks * 8192); // 8KiB
            stream.read((char*)CHR_Memory.data(), CHR_Memory.size());
        }

        switch (mapperId)
        {
        case 0:         mapper = std::make_shared<Mapper_000>(header.prg_rom_chunks, header.chr_rom_chunks);        break;
        default:
            assert(false, "Mapper Not Supported");
            break;
        }

        stream.close();

    }

    void Cartridge::load(std::vector<Byte>& memoryDump)
    {
        assert(memoryDump.size() < nes::Memory::MAX_MEM, "The Memory dump passed is bigger than supported size");
        memoryDump.resize(nes::Memory::MAX_MEM);
        Memory* memory = bus->getMicroprocesor<Memory>();
        assert(memory != nullptr, "Couldn't get Memory connected to the bus");
        assert(memoryDump.size() == Memory::MAX_MEM, "Couldn't get Memory connected to the bus");
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
