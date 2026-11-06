#pragma once

#include "neat/genome.h"

namespace ab
{
    class NeuralNetworkVisualizer
    {
    public:
        static void render(bool& open, const Genome& genome);
    };
}
