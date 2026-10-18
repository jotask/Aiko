
#pragma once

#include <array>
#include <deque>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <aiko_types.h>

#include "models/material.h"
#include "models/render_target.h"
#include "post_process_effect.h"

namespace aiko::renderer
{
    class IRenderDevice;
}

namespace aiko
{
    class PostProcessPipeline
    {
    public:
        PostProcessPipeline() = default;
        ~PostProcessPipeline() = default;

        PostProcessPipeline(const PostProcessPipeline&) = delete;
        PostProcessPipeline& operator=(const PostProcessPipeline&) = delete;

        template<typename T, typename... Args>
        T& addEffect(Args&&... args)
        {
            static_assert(std::is_base_of_v<PostProcessEffect, T>, "T must derive from PostProcessEffect");

            auto effect = std::make_unique<T>(std::forward<Args>(args)...);

            T& result = *effect;
            addEffect(std::move(effect));
            return result;
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
        void dispose();

        const Texture& execute(renderer::IRenderDevice& device, std::deque<Material>& frameMaterials, const RenderTarget& scene);

        const std::vector<std::unique_ptr<PostProcessEffect>>& effects() const { return m_effects; }

    private:
        using EffectList = std::vector<std::unique_ptr<PostProcessEffect>>;

        EffectList::iterator findIterator(const std::string& name);
        EffectList::const_iterator findIterator(const std::string& name) const;

        RenderTarget& acquireTarget(PostProcessDomain domain, const Texture& input, u32 width,u32 height);

        EffectList m_effects;

        std::array<std::unique_ptr<RenderTarget>, 2> m_hdrTargets;
        std::array<std::unique_ptr<RenderTarget>, 2> m_ldrTargets;

        u32 m_width = 0;
        u32 m_height = 0;
    };
}
