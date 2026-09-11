#include "selection_sort.h"

#include "sort_lab.h"

namespace sb
{
    SelectionSort::SelectionSort(SortLab* lab)
        : Sorter(lab, "SelectionSort")
    {
    }

    void SelectionSort::init()
    {
        i = 0;
        j = 1;
        minIndex = 0;
    }

    void SelectionSort::step(Numeros& n)
    {
        const auto arr_size = n.size();

        if (i >= arr_size - 1)
        {
            return;
        }

        if (n[j] < n[minIndex])
        {
            minIndex = j;
        }

        j++;

        if ( j >= arr_size)
        {
            std::swap(n[i], n[minIndex]);
            i++;
            minIndex = i;
            j = i + 1;
            return;
        }

    }

    void SelectionSort::dispose()
    {
    }
}
