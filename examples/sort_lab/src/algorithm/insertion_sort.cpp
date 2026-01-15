#include "insertion_sort.h"

#include "sort_lab.h"

namespace sb
{
    InsertionSort::InsertionSort(SortLab* lab)
        : Sorter(lab, "SelectionSort")
    {
    }

    void InsertionSort::init()
    {
        i = 1;
        j = -1;
        key = 0;
        innerLoop = false;
    }

    void InsertionSort::step(Numeros& n)
    {
        const int arr_size = static_cast<int>(n.size());

        if (i >= arr_size)
        {
            return;
        }

        // AIKO_NOT_IMPLEMENTED;

    }

    void InsertionSort::dispose()
    {
    }
}
