#pragma once

#include <vector>

#include "nes/microprocessor.h"
#include "nes/nes_types.h"

namespace nes
{
    class Nes;
    class Bus : public Microprocessor
    {
    public:
        friend class Nes;
        Bus();
        virtual void reset() override;
        void addMicroprocesor(Microprocessor*);

        template<class T>
        T* getMicroprocesor();

        virtual void clock() override;

        void cpu_write(Word, Byte);
        Byte cpu_read(Word, bool = false);

    private:
        std::vector<Microprocessor*> microprocessors;
        Nes* m_nes;

        Byte cpu_ram[2048];

    };

    template<class T>
    inline T* Bus::getMicroprocesor()
    {
        static_assert(std::is_base_of<Microprocessor, T>::value, "T must derive from Microprocessor");
        auto find = [](Microprocessor* micro)
        {
            return dynamic_cast<T*>(micro) != nullptr;
        };
        auto it = std::find_if(microprocessors.begin(), microprocessors.end(), find );
        return (it != microprocessors.end()) ? dynamic_cast<T*>(*it) : nullptr;
    }

}
