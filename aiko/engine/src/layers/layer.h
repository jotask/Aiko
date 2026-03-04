#pragma once

namespace aiko
{
    class Layer
    {
    public:
        Layer() = default;
        virtual ~Layer() = default;

        virtual void onAttach() = 0;
        virtual void onDetach() = 0;

        virtual void onUpdate() = 0;
        virtual void onRender() = 0;
        virtual void onEvent(Event& e) = 0;

    };
}