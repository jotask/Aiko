#pragma once

namespace nes
{

    class Microprocessor
    {
        friend class Bus;
    public:
        virtual void reset() = 0;
        virtual void clock() { };
    protected:
        Bus* bus;
    };

}
