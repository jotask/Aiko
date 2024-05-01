#pragma once

#include <vector>

#include "microprocessor.h"

namespace nes
{
    class nes6502;
    class Bus : public Microprocessor
    {
    public:
        friend class nes6502;
        Bus();
        virtual void reset() override;
        void addMicroprocesor(Microprocessor*);

        template<class T>
        T* getMicroprocesor();

        virtual void clock() override;

    private:
        std::vector<Microprocessor*> microprocessors;
        nes6502* m_nes;

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
