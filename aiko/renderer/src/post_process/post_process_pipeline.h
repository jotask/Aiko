#pragma once

#include <memory>
#include <string>
#include <vector>
#include <type_traits>
#include <utility>

#include <aiko_types.h>

#include "post_process_effect.h"

namespace aiko
{
    class PostProcessPipeline
    {
    public:
        PostProcessPipeline() = default;
        ~PostProcessPipeline() = default;

        PostProcessPipeline(const PostProcessPipeline&) = delete;
        PostProcessPipeline& operator=(const PostProcessPipeline&) = delete;

        PostProcessPipeline(PostProcessPipeline&&) noexcept = delete;
        PostProcessPipeline& operator=(PostProcessPipeline&&) noexcept = delete;

        template<typename T, typename... Args>
        T& addEffect(Args&&... args)
        {
            static_assert(std::is_base_of_v<PostProcessEffect, T>, "T must derive from PostProcessEffect");

            auto effect = std::make_unique<T>(std::forward<Args>(args)...);
            T* ptr = effect.get();

            AIKO_ASSERT(findEffect(ptr->name()) == nullptr, "Duplicate post-process effect name");

            m_effects.push_back(std::move(effect));
            return *ptr;
        }

        void addEffect(std::unique_ptr<PostProcessEffect> effect);

        bool removeEffect(const std::string& name);
        void clear();

        PostProcessEffect* findEffect(const std::string& name);
        const PostProcessEffect* findEffect(const std::string& name) const;

        bool setEnabled(const std::string& name, bool enabled);

        bool moveBefore(const std::string& effectName, const std::string& beforeName);
        bool moveAfter(const std::string& effectName, const std::string& afterName);

        void resize(u32 width, u32 height);

        const std::vector<std::unique_ptr<PostProcessEffect>>& effects() const
        {
            return m_effects;
        }

    private:
        std::vector<std::unique_ptr<PostProcessEffect>>::iterator findIterator(const std::string& name);
        std::vector<std::unique_ptr<PostProcessEffect>>::const_iterator findIterator(const std::string& name) const;

    private:
        std::vector<std::unique_ptr<PostProcessEffect>> m_effects;
    };
}
