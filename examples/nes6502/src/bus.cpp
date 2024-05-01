#include "bus.h"
namespace nes
{
    Bus::Bus()
    {
    }

    void Bus::reset()
    {
        microprocessors.clear();
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
