#pragma once

#include "sorter.h"

namespace sb
{

    class SelectionSort : public Sorter
    {

    public:

        SelectionSort(SortLab*);
        virtual ~SelectionSort() override = default;

        virtual void init() override;
        virtual void step(Numeros& n) override;
        virtual void dispose() override;

    private:

        uint16_t i;
        uint16_t j;
        uint16_t minIndex;

    };

}
