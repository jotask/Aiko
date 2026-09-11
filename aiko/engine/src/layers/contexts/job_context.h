#pragma once

#include <functional>
#include <future>
#include <type_traits>
#include <utility>

#include "types/job_types.h"

namespace aiko
{
    class SystemConnector;
    class JobSystem;

    class JobContext
    {
    public:
        template<typename Work>
        void submit(Work&& work)
        {
            using WorkType = std::decay_t<Work>;

            enqueueBackground(
                std::packaged_task<void()>(
                    [work = WorkType(std::forward<Work>(work))]() mutable
                    {
                        std::invoke(work);
                    }
                )
            );
        }

        template<typename Work, typename Completion>
        void submit(Work&& work, Completion&& completion)
        {
            using WorkType = std::decay_t<Work>;
            using CompletionType = std::decay_t<Completion>;
            using Result = std::invoke_result_t<WorkType&>;

            static_assert(std::is_void_v<Result> == false, "JobContext::submit(work, completion) requires work to return a value");

            JobContext* self = this;

            enqueueBackground(
                std::packaged_task<void()>(
                    [
                        self,
                        work = WorkType(std::forward<Work>(work)),
                        completion = CompletionType(std::forward<Completion>(completion))
                    ]() mutable
                    {
                        Result result = std::invoke(work);

                        self->enqueueMainThread(
                            std::packaged_task<void()>(
                                [
                                    completion = std::move(completion),
                                    result = std::move(result)
                                ]() mutable
                                {
                                    std::invoke(
                                        completion,
                                        std::move(result)
                                    );
                                }
                            )
                        );
                    }
                )
            );
        }

        template<typename Work>
        void dispatchMainThread(Work&& work)
        {
            using WorkType = std::decay_t<Work>;

            enqueueMainThread(
                std::packaged_task<void()>(
                    [work = WorkType(std::forward<Work>(work))]() mutable
                    {
                        std::invoke(work);
                    }
                )
            );
        }

        JobStats stats() const;

    private:
        friend class LayerContext;

        explicit JobContext(SystemConnector& connector);

        void enqueueBackground(std::packaged_task<void()> job);
        void enqueueMainThread(std::packaged_task<void()> job);

    private:
        JobSystem* m_jobSystem = nullptr;
    };
}
