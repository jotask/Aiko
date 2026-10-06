#pragma once

#include "models/font.h"
#include "sort_types.h"
#include "sorter.h"

#include <layers/layer.h>

#include <aiko_types.h>
#include <array>

namespace sb
{

    class SortLab : public aiko::Layer
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

        void  printArray();

        uint16_t m_currentSorterIdx;
        float m_timer;

        aiko::Font m_debugFont;

    };

}

