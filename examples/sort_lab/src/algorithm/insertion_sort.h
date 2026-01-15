#pragma once

#include "sorter.h"

namespace sb
{

    class InsertionSort : public Sorter
    {

    public:

        InsertionSort(SortLab*);
        virtual ~InsertionSort() override = default;

        virtual void init() override;
        virtual void step(Numeros& n) override;
        virtual void dispose() override;

    private:

        uint16_t i;
        uint16_t j;
        NUMBER key;
        bool innerLoop;

    };

}
