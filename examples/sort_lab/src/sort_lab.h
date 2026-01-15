#pragma once

#include "sort_types.h"
#include "sorter.h"

#include <array>

#include "aiko_types.h"
#include "application/application.h"

namespace sb
{

    class SortLab : public aiko::Application
    {

    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;

        void nextSorter(int dir);

        bool isSorted() const;

    private:

         Numeros m_numbers;

        std::vector<aiko::AikoPtr<Sorter>> m_sorters;

        void setup();

        void shuffle();
        void clear();

        uint16_t m_currentSorterIdx;
        float m_timer;

    };

}

