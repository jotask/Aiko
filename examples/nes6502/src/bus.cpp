#include "bus.h"
namespace nes
{
    void Bus::reset()
    {
        microprocessors.clear();
    }

    void Bus::addMicroprocesor(Microprocessor* m)
    {
        m->bus = this;
        microprocessors.push_back(m);
    }
}