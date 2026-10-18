#include "post_process_pipeline.h"

#include "post_process_context.h"
#include "renderer/Irenderdevice.h"

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
        AIKO_ASSERT(width > 0 && height > 0, "Invalid post-process dimensions");

        if (m_width == width && m_height == height)
        {
            return;
        }

        m_width = width;
        m_height = height;

        // Targets are recreated lazily with their required formats.
        for (auto& target : m_hdrTargets)
        {
            if (target)
            {
                target->unload();
            }
        }

        for (auto& target : m_ldrTargets)
        {
            if (target)
            {
                target->unload();
            }
        }

        for (auto& effect : m_effects)
        {
            effect->resize(width, height);
        }
    }

    void PostProcessPipeline::dispose()
    {
        clear();

        for (auto& target : m_hdrTargets)
        {
            if (target)
            {
                target->unload();
                target.reset();
            }
        }

        for (auto& target : m_ldrTargets)
        {
            if (target)
            {
                target->unload();
                target.reset();
            }
        }

        m_width = 0;
        m_height = 0;
    }

    RenderTarget& PostProcessPipeline::acquireTarget(PostProcessDomain domain, const Texture& input, u32 width, u32 height)
    {
        auto& targets = domain == PostProcessDomain::HDR ? m_hdrTargets : m_ldrTargets;

        const TextureFormat format = domain == PostProcessDomain::HDR ? TextureFormat::RGBA16F : TextureFormat::RGBA8;

        for (auto& slot : targets)
        {
            if (!slot)
            {
                slot = std::make_unique<RenderTarget>();
            }

            if (slot->isValid() &&
                &slot->colorTexture() == &input)
            {
                continue;
            }

            if (!slot->isValid())
            {
                slot->create(width, height, format);
            }
            else
            {
                const ivec2 size = slot->size();

                if (size.x != static_cast<int>(width) || size.y != static_cast<int>(height))
                {
                    slot->resize(width, height);
                }
            }

            AIKO_ASSERT(slot->isValid(), "Failed to create post-process target");

            return *slot;
        }

        AIKO_ASSERT(false, "No free post-process output target");

        std::abort();
    }

    const Texture& PostProcessPipeline::execute(renderer::IRenderDevice& device, std::deque<Material>& frameMaterials, const RenderTarget& scene)
    {
        AIKO_ASSERT(scene.isValid(), "Post-processing requires a valid scene target");

        const ivec2 size = scene.size();

        AIKO_ASSERT(size.x > 0 && size.y > 0, "Invalid scene target dimensions");

        const u32 width = static_cast<u32>(size.x);
        const u32 height = static_cast<u32>(size.y);

        if (width != m_width || height != m_height)
        {
            resize(width, height);
        }

        const Texture* current = &scene.colorTexture();
        PostProcessDomain currentDomain = PostProcessDomain::HDR;

        bool transitionedToLdr = false;

        for (const auto& effect : m_effects)
        {
            AIKO_ASSERT(effect != nullptr, "Null post-process effect");

            if (!effect->enabled())
            {
                continue;
            }

            AIKO_ASSERT(effect->inputDomain() == currentDomain, "Invalid post-process input domain");

            if (effect->inputDomain() == PostProcessDomain::LDR && effect->outputDomain() == PostProcessDomain::HDR)
            {
                AIKO_ASSERT(false, "Post-processing cannot transition from LDR to HDR");
            }

            if (currentDomain == PostProcessDomain::HDR && effect->outputDomain() == PostProcessDomain::LDR)
            {
                AIKO_ASSERT(!transitionedToLdr, "Duplicate HDR-to-LDR transition");
                transitionedToLdr = true;
            }

            RenderTarget& output = acquireTarget(effect->outputDomain(), *current, width, height);

            PostProcessContext context(device, frameMaterials, *current, scene.depthTexture(), output);

            effect->execute(context);

            current = &output.colorTexture();
            currentDomain = effect->outputDomain();
        }

        return *current;
    }

    std::vector<std::unique_ptr<PostProcessEffect>>::iterator  PostProcessPipeline::findIterator(const std::string& name)
    {
        return std::find_if(
            m_effects.begin(),
            m_effects.end(),
            [&](const std::unique_ptr<PostProcessEffect>& effect)
            {
                return effect != nullptr && effect->name() == name;
            });
    }

    std::vector<std::unique_ptr<PostProcessEffect>>::const_iterator PostProcessPipeline::findIterator(const std::string& name) const
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
