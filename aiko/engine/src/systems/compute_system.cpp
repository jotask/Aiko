#include "compute_system.h"

#include "systems/system_connector.h"
#include "systems/scene_system.h"
#include "systems/render_system.h"

namespace aiko
{
    ComputeSystem::ComputeSystem()
        : m_renderSystem(nullptr)
        , m_sceneSystem(nullptr)
    {
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
            ;
            if (auto cmp = object->getComponent<ComputeShaderComponent>())
            {
                updateComponent(object, *cmp);
            }
        }
    }

    void ComputeSystem::dispose()
    {
        destroyStates();
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
            state.buffer.createVec4(count, nullptr, ComputeAccess::ReadWrite);
            state.initialized = true;
            state.dispatched = false;
            state.readbackRequested = false;
            state.readbackId = 0;
        }

        if (state.dispatched == false)
        {
            ComputePass pass = {};
            pass.buffers.push_back({ 0, &state.buffer, ComputeAccess::ReadWrite});
            pass.vec4Uniforms.push_back({ "u_params", vec4(float(count), 0.0f, 0.0f, 0.0f) });
            pass.dispatch.groupsX = (count + 63) / 64;
            pass.dispatch.groupsY = 1;
            pass.dispatch.groupsZ = 1;

            m_renderSystem->dispatch(pass, shaderId);
            state.dispatched = true;

        }

        if (cmp.consumeReadbackRequest() == true && state.readbackRequested == false)
        {
            state.readbackId = m_nextReadbackId++;

            ComputeReadbackRequest req = {};
            req.id = state.readbackId;
            req.buffer = &state.buffer;
            req.byteSize =  count * sizeof(vec4);
            m_renderSystem->requestReadback(req);
            state.readbackRequested = true;

        }

        if (state.readbackRequested == true)
        {
            ComputeReadbackResult result = {};
            m_renderSystem->pollReadback(result);
            if (result.ready == true && result.id == state.readbackId)
            {
                cmp.setLastReadback(std::move(result));
                state.readbackRequested = false;
            }
        }

    }

    ComputeSystem::RuntimeState& ComputeSystem::getOrCreateState(const ComputeShaderComponent* cmp)
    {
        auto it = m_runtime.find(cmp);
        if (it != m_runtime.end())
        {
            return *it->second;
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
        }
        m_runtime.clear();
    }
}
