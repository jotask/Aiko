#include "bus.h"
namespace nes
{
    Bus::Bus()
    {
    }

    void Bus::reset()
    {
        for (auto& m : microprocessors)
        {
            m->reset();
        }
    }

    void Bus::addMicroprocesor(Microprocessor* m)
    {
        m->bus = this;
        microprocessors.push_back(m);
    }

    void Bus::clock()
    {
        for(auto& m : microprocessors )
        {
            m->clock();
        }
    }
}
