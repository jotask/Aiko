#include "bubble_sort.h"

#include "logger/logger.h"

#include "sort_lab.h"

namespace sb
{
    BubbleSort::BubbleSort(SortLab* lab)
        : Sorter(lab, "BubbleSort")
    {
    }

    void BubbleSort::init()
    {
        aiko::logger::Log::info("init()");
        i = 0;
        j = 0;
    }

    void BubbleSort::step(Numeros& n)
    {
        aiko::logger::Log::info("step()");

        const auto arr_size = n.size() - 1;

        if ( j >= arr_size - i && i >= arr_size)
        {
            return;
        }

        if (j >= arr_size - i)
        {
            j = 0;
            i++;
        }

        if (n[j]> n[j+1])
        {
            std::swap(n[j], n[j+1]);
        }
        j++;
    }

    void BubbleSort::dispose()
    {
        aiko::logger::Log::info("dispose()");
    }
}
