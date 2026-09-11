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
        isInserting = false;
    }

    void InsertionSort::step(Numeros& n)
    {
        const int arr_size = static_cast<int>(n.size());

        if (i >= arr_size)
        {
            return;
        }

        if (isInserting == false)
        {
            key = n[i];
            j = i - 1;
            isInserting = true;
            return;
        }

        if (j >= 0 && n[j] > key)
        {
            n[j + 1] = n[j];
            --j;
            return;
        }
        n[j + 1] = key;
        ++i;
        isInserting = false;
    }

    void InsertionSort::dispose()
    {
    }
}
