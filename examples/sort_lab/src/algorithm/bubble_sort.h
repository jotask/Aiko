#pragma once

#include "sorter.h"

namespace sb
{

    class BubbleSort : public Sorter
    {

    public:

        BubbleSort(SortLab* lab);
        virtual ~BubbleSort() override = default;

        virtual void init() override;
        virtual void step(Numeros& n) override;
        virtual void dispose() override;

    private:

        uint16_t i;
        uint16_t j;

    };

}