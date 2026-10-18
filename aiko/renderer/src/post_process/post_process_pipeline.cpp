#include "post_process_pipeline.h"

#include <algorithm>

namespace aiko
{
    void PostProcessPipeline::addEffect(std::unique_ptr<PostProcessEffect> effect)
    {
        AIKO_ASSERT(effect != nullptr, "Post-process effect cannot be null");
        AIKO_ASSERT(findEffect(effect->name()) == nullptr, "Duplicate post-process effect name");

        m_effects.push_back(std::move(effect));
    }

    bool PostProcessPipeline::removeEffect(const std::string& name)
    {
        const auto it = findIterator(name);

        if (it == m_effects.end())
        {
            return false;
        }

        m_effects.erase(it);
        return true;
    }

    void PostProcessPipeline::clear()
    {
        m_effects.clear();
    }

    PostProcessEffect* PostProcessPipeline::findEffect(const std::string& name)
    {
        const auto it = findIterator(name);
        return it != m_effects.end() ? it->get() : nullptr;
    }

    const PostProcessEffect* PostProcessPipeline::findEffect(const std::string& name) const
    {
        const auto it = findIterator(name);
        return it != m_effects.end() ? it->get() : nullptr;
    }

    bool PostProcessPipeline::setEnabled(const std::string& name, bool enabled)
    {
        PostProcessEffect* effect = findEffect(name);

        if (effect == nullptr)
        {
            return false;
        }

        effect->setEnabled(enabled);
        return true;
    }

    bool PostProcessPipeline::moveBefore(const std::string& effectName, const std::string& beforeName)
    {
        if (effectName == beforeName)
        {
            return false;
        }

        auto from = findIterator(effectName);
        auto to = findIterator(beforeName);

        if (from == m_effects.end() || to == m_effects.end())
        {
            return false;
        }

        auto node = std::move(*from);
        m_effects.erase(from);

        to = findIterator(beforeName);
        m_effects.insert(to, std::move(node));
        return true;
    }

    bool PostProcessPipeline::moveAfter(const std::string& effectName, const std::string& afterName)
    {
        if (effectName == afterName)
        {
            return false;
        }

        auto from = findIterator(effectName);
        auto to = findIterator(afterName);

        if (from == m_effects.end() || to == m_effects.end())
        {
            return false;
        }

        auto node = std::move(*from);
        m_effects.erase(from);

        to = findIterator(afterName);
        ++to;
        m_effects.insert(to, std::move(node));
        return true;
    }

    void PostProcessPipeline::resize(u32 width, u32 height)
    {
        for (const auto& effect : m_effects)
        {
            AIKO_ASSERT(effect != nullptr, "Post-process effect is null");
            effect->resize(width, height);
        }
    }

    std::vector<std::unique_ptr<PostProcessEffect>>::iterator
    PostProcessPipeline::findIterator(const std::string& name)
    {
        return std::find_if(
            m_effects.begin(),
            m_effects.end(),
            [&](const std::unique_ptr<PostProcessEffect>& effect)
            {
                return effect != nullptr && effect->name() == name;
            });
    }

    std::vector<std::unique_ptr<PostProcessEffect>>::const_iterator
    PostProcessPipeline::findIterator(const std::string& name) const
    {
        return std::find_if(
            m_effects.begin(),
            m_effects.end(),
            [&](const std::unique_ptr<PostProcessEffect>& effect)
            {
                return effect != nullptr && effect->name() == name;
            });
    }
}
