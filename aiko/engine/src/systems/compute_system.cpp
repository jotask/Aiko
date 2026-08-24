#include "compute_system.h"

#include "systems/system_connector.h"
#include "systems/scene_system.h"
#include "systems/render_system.h"
#include "time/time.h"

namespace aiko
{

    ComputeSystem::ComputeSystem()
        : m_renderSystem(nullptr)
        , m_sceneSystem(nullptr)
    {
    }

    const Texture* ComputeSystem::getOutputTexture(const ComputeShaderComponent* component) const
    {
        const RuntimeState* state = tryGetState(component);
        if (state == nullptr)
        {
            return nullptr;
        }
        if (state->output.isValid() == false)
        {
            return nullptr;
        }
        return &state->output;
    }

    void ComputeSystem::update()
    {
        const auto& objects = m_sceneSystem->getScene().getObjects();
        for (const auto& object : objects)
        {
            if (object == nullptr)
            {
                continue;
            }
            if (auto cmp = object->getComponent<ComputeShaderComponent>())
            {
                updateComponent(object, *cmp);
            }
        }
    }

    void ComputeSystem::render()
    {
        const auto& objects = m_sceneSystem->getScene().getObjects();
        for (const auto& object : objects)
        {
            if (object == nullptr)
            {
                continue;
            }
            if (auto cmp = object->getComponent<ComputeShaderComponent>())
            {
                renderComponent(object, *cmp);
            }
        }
    }

    void ComputeSystem::dispose()
    {
        destroyStates();
        BaseSystem::dispose();
    }

    void ComputeSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_SYSTEM_REQUIRED(RenderSystem, systemConnector, m_renderSystem);
        BIND_SYSTEM_REQUIRED(SceneSystem, systemConnector, m_sceneSystem);
    }

    void ComputeSystem::updateComponent(GameObject* obj, ComputeShaderComponent& cmp)
    {
        AIKO_UNUSED(obj);

        // TODO only enabled components

        const AssetId& shaderId = cmp.getShaderId();
        if (shaderId == InvalidAssetId)
        {
            return;
        }

        RuntimeState& state = getOrCreateState(&cmp);

        const uint32_t count = cmp.getElementCount();

        if (state.initialized == false)
        {
            if (cmp.usesOutputTexture() == false)
            {
                state.buffer.createVec4(count, nullptr);
            }

            state.initialized = true;
            state.dispatched = false;
            state.readbackRequested = false;
            state.readbackId = 0;
        }

        if (cmp.usesOutputTexture() == true)
        {
            const uint32_t width = cmp.getOutputWidth();
            const uint32_t height = cmp.getOutputHeight();

            static auto needsOutputResize = [](const RuntimeState& state, uint32_t width, uint32_t height) -> bool
            {
                if (state.output.isValid() == false)
                {
                    return true;
                }

                return state.outputWidth != width || state.outputHeight != height;
            };
            if (needsOutputResize(state, width, height) == true)
            {
                if (state.output.isValid() == true)
                {
                    state.output.unload();
                }

                const TextureDesc desc =
                {
                    .type = TextureType::Sampled,
                    .format = TextureFormat::RGBA8,
                    .width = static_cast<int>(width),
                    .height = static_cast<int>(height),
                    .mipmaps = false,
                    .computeWrite = true,
                };

                AIKO_ASSERT(width > 0 && height > 0, "Compute output texture size must be > 0");

                state.output.create(desc);
                state.outputWidth = width;
                state.outputHeight = height;
                state.dispatched = false;
            }
        }

        if (cmp.usesOutputTexture() == true)
        {
            if (auto spriteCmp = obj->getComponent<SpriteComponent>())
            {
                if (state.output.isValid() == true)
                {
                    spriteCmp->getMaterialInstance().runtimeDiffuseTexture = &state.output;
                }
            }
        }

    }

    void ComputeSystem::renderComponent(GameObject* obj, ComputeShaderComponent& cmp)
    {

        RuntimeState* state = tryGetState(&cmp);

        if (state == nullptr)
        {
            return;
        }

        const uint32_t count = cmp.getElementCount();

        const AssetId& shaderId = cmp.getShaderId();
        if (shaderId == InvalidAssetId)
        {
            return;
        }

        if (shouldDispatch(cmp, *state) == true)
        {
            ComputePass pass = {};
            ComputeParams params{};

            if (cmp.usesOutputTexture() == true)
            {
                static float sinceStart = 0.0f;
                sinceStart += Time::it().getDeltaTime();
                params.u_params = vec4(sinceStart, 0.0f, 0.0f, 0.0f);
            }
            else
            {
                params.u_params = vec4( static_cast<float>(count), 0.0f, 0.0f, 0.0f);
            }

            pass.setPushConstants(params);

            if (cmp.usesOutputTexture() == false)
            {
                pass.buffers.push_back({ 0, &state->buffer, ComputeAccess::ReadWrite });
            }

            if (cmp.usesOutputTexture() == true)
            {
                const uint32_t width = cmp.getOutputWidth();
                const uint32_t height = cmp.getOutputHeight();

                pass.dispatch.groupsX = (width + 7) / 8;
                pass.dispatch.groupsY = (height + 7) / 8;
                pass.dispatch.groupsZ = 1;
            }
            else
            {
                pass.dispatch.groupsX = (count + 63) / 64;
                pass.dispatch.groupsY = 1;
                pass.dispatch.groupsZ = 1;
            }

            if (cmp.usesOutputTexture() == true)
            {
                pass.images.push_back({0, &state->output, ComputeAccess::Write });
            }

            m_renderSystem->dispatch(pass, shaderId);
            state->dispatched = true;

        }

        if (cmp.usesOutputTexture() == false && cmp.consumeReadbackRequest() == true && state->readbackRequested == false)
        {
            state->readbackId = m_nextReadbackId++;

            AIKO_ASSERT(state->buffer.isValid(), "Attempting readback from invalid compute buffer");

            ComputeReadbackRequest req = {};
            req.id = state->readbackId;
            req.buffer = &state->buffer;
            req.byteSize =  count * sizeof(vec4);
            m_renderSystem->requestReadback(req);
            state->readbackRequested = true;

        }

        if (state->readbackRequested == true)
        {
            ComputeReadbackResult result = {};
            if (m_renderSystem->pollReadback(result) == true)
            {
                if (result.ready == true && result.id == state->readbackId)
                {
                    cmp.setLastReadback(std::move(result));
                    state->readbackRequested = false;
                }
            }
        }

    }

    ComputeSystem::RuntimeState* ComputeSystem::tryGetState(const ComputeShaderComponent* cmp)
    {
        auto it = m_runtime.find(cmp);
        if (it != m_runtime.end())
        {
            return it->second.get();
        }
        return nullptr;
    }

    const ComputeSystem::RuntimeState* ComputeSystem::tryGetState(const ComputeShaderComponent* cmp) const
    {
        auto it = m_runtime.find(cmp);
        if (it != m_runtime.end())
        {
            return it->second.get();
        }
        return nullptr;
    }

    ComputeSystem::RuntimeState& ComputeSystem::getOrCreateState(const ComputeShaderComponent* cmp)
    {
        if (RuntimeState* state = tryGetState(cmp))
        {
            return *state;
        }
        auto state = std::make_unique<RuntimeState>();
        RuntimeState& ref = *state;
        m_runtime.emplace(cmp, std::move(state));
        return ref;
    }

    void ComputeSystem::destroyStates()
    {
        for (auto& state : m_runtime)
        {
            state.second->buffer.unload();
            state.second->output.unload();
        }
        m_runtime.clear();
    }

    bool ComputeSystem::shouldDispatch(ComputeShaderComponent& cmp, RuntimeState& state)
    {
        switch (cmp.getExecutionMode())
        {
        case ComputeShaderComponent::ComputeExecutionMode::Once:
            {
                return state.dispatched == false;
            }

        case ComputeShaderComponent::ComputeExecutionMode::Continuous:
            {
                const float interval = cmp.getUpdateInterval();

                if (interval <= 0.0f)
                {
                    return true;
                }

                const auto dt = Time::it().getDeltaTime();

                state.accumulator += dt;
                if (state.accumulator >= interval)
                {
                    state.accumulator = 0.0f;
                    return true;
                }

                return false;
            }

        case ComputeShaderComponent::ComputeExecutionMode::OnDemand:
            {
                return cmp.consumeDispatchRequest();
            }
        }

        return false;
    }
}
