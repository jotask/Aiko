#include "job_context.h"

#include "aiko_macros.h"
#include "systems/system_connector.h"
#include "systems/job_system.h"

namespace aiko
{
    JobContext::JobContext(SystemConnector& connector)
    {
        m_jobSystem = connector.find<JobSystem>();
        AIKO_ASSERT(m_jobSystem != nullptr, "Required system JobSystem not found");
    }

    void JobContext::enqueueBackground(std::packaged_task<void()> job)
    {
        AIKO_ASSERT(m_jobSystem != nullptr, "JobContext has no JobSystem");
        m_jobSystem->submit(std::move(job));
    }

    void JobContext::enqueueMainThread(std::packaged_task<void()> job)
    {
        AIKO_ASSERT(m_jobSystem != nullptr, "JobContext has no JobSystem");
        m_jobSystem->dispatchMainThread(std::move(job));
    }

    JobStats JobContext::stats() const
    {
        AIKO_ASSERT(m_jobSystem != nullptr, "JobContext has no JobSystem");
        return m_jobSystem->stats();
    }
}
