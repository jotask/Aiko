#pragma once

#include "sort_types.h"

#include <array>

#include "application/application.h"

namespace sb
{

    class SortLab : public aiko::Application
    {
    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
    private:

        std::array<NUMBER, MAX_VALUE> numbers;

        void shuffle();
        void clear();
        void sort();

    };

}

