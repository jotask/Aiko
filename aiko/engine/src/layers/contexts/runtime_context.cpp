#include "runtime_context.h"

#include "aiko.h"

namespace aiko
{

    bool RuntimeContext::isPaused() const
    {
        AIKO_ASSERT(m_runtime != nullptr, "RuntimeContext has no Aiko runtime");
        return m_runtime->isSimulationPaused();
    }

    void RuntimeContext::setPaused(bool paused)
    {
        AIKO_ASSERT(m_runtime != nullptr, "RuntimeContext has no Aiko runtime");

        if (m_runtime != nullptr)
        {
            m_runtime->setSimulationPaused(paused);
        }
    }

    void RuntimeContext::step()
    {
        AIKO_ASSERT(m_runtime != nullptr, "RuntimeContext has no Aiko runtime");

        if (m_runtime != nullptr)
        {
            m_runtime->stepSimulation();
        }
    }

}
