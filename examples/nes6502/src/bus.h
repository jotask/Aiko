#pragma once

#include <vector>

#include "microprocessor.h"

namespace nes
{
    class Bus : public Microprocessor
    {
    public:
        virtual void reset() override;
        void addMicroprocesor(Microprocessor*);

        template<class T>
        T* getMicroprocesor();

    private:
        std::vector<Microprocessor*> microprocessors;

    };

    template<class T>
    inline T* Bus::getMicroprocesor()
    {
        auto find = [](Microprocessor* micro)
        {
            return dynamic_cast<T*>(micro) != nullptr;
        };
        auto it = std::find_if(microprocessors.begin(), microprocessors.end(), find );
        return (it != microprocessors.end()) ? dynamic_cast<T*>(it) : nullptr;
    }

}
