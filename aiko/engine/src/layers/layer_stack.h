#pragma once

#include "layers/layer.h"

#include <aiko_types.h>

namespace aiko
{

    class LayerStack
    {
    public:

        LayerStack();
        ~LayerStack();

        Layer* pushLayer(AikoUPtr<Layer> layer);
        Layer* pushOverlay(AikoUPtr<Layer> layer);
        void popLayer(Layer*);
        void popOverlay(Layer*);

        auto begin() { return m_layers.begin(); }
        auto end(){ return m_layers.end(); }

        auto rbegin(){ return m_layers.rbegin(); }
        auto rend(){ return m_layers.rend(); }

    private:

        vector<AikoUPtr<Layer>> m_layers;
        size_t m_insertIndex;

    };
}