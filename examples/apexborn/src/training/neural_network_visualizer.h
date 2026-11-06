#pragma once

#include "neat/genome.h"
#include "neat/neural_network.h"

#include <span>

namespace ab
{
    class NeuralNetworkVisualizer
    {
    public:
        static void render(bool& open, const Genome& genome, std::span<const NodeActivation> activations = {}, const char* modeLabel = "Neural Network");
    };
}
