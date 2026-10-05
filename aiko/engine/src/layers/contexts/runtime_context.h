#pragma once

namespace aiko
{
    class Aiko;

    class RuntimeContext
    {
        friend class LayerContext;

    public:
        bool isPaused() const;
        void setPaused(bool paused);
        void step();

    private:
        explicit RuntimeContext(Aiko& runtime)
            : m_runtime(&runtime)
        {
        }

        Aiko* m_runtime = nullptr;
    };
}
