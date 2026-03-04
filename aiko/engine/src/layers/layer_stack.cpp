#include "layer_stack.h"

#include <algorithm>
#include <cstddef>

namespace aiko
{
    LayerStack::LayerStack()
        : m_insertIndex(0)
    {
    }

    LayerStack::~LayerStack()
    {
        for (const auto& layer : m_layers)
        {
            layer->dispose();
            layer->onDetach();
        }
    }

    Layer* LayerStack::pushLayer(AikoUPtr<Layer> layer)
    {
        // Add before overlays
        auto it = m_layers.begin() + static_cast<std::ptrdiff_t>(m_insertIndex);
        it = m_layers.insert(it, std::move(layer));
        (*it)->onAttach();
        m_insertIndex++;
        return (*it).get();
    }

    Layer* LayerStack::pushOverlay(AikoUPtr<Layer> layer)
    {
        m_layers.push_back(std::move(layer));
        m_layers.back()->onAttach();
        return m_layers.back().get();
    }

    void LayerStack::popLayer(Layer* ptr)
    {
        const auto layerEnd = m_layers.begin() + static_cast<std::ptrdiff_t>(m_insertIndex);
        auto found = std::find_if(
            m_layers.begin(),
            layerEnd,
            [ptr](const AikoUPtr<Layer>& tmp)
        {
            return tmp.get() == ptr;
        });
        if (found != layerEnd)
        {
            (*found)->dispose();
            (*found)->onDetach();
            m_layers.erase(found);
            m_insertIndex--;
        }
    }

    void LayerStack::popOverlay(Layer* ptr)
    {
        auto found = std::find_if(
            m_layers.begin() + static_cast<std::ptrdiff_t>(m_insertIndex),
            m_layers.end(),
            [ptr](const AikoUPtr<Layer>& tmp)
        {
            return tmp.get() == ptr;
        });
        if (found != m_layers.end())
        {
            (*found)->dispose();
            (*found)->onDetach();
            m_layers.erase(found);
        }
    }
}
