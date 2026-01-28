#pragma once

#include <aiko.h>

#include <sort_types.h>

namespace sb
{

    class SortLab;
    class Sorter
    {
    public:
        Sorter(SortLab* lab, aiko::string name) : m_lab(lab), m_name(name) {};
        virtual ~Sorter() = default;

        virtual void init() = 0;
        virtual void step(Numeros& n) = 0;
        virtual void dispose() = 0;

    protected:
        SortLab* m_lab;
        aiko::string m_name;
    };

}