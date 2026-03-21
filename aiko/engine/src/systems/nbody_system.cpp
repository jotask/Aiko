#include "nbody_system.h"

#include <time/time.h>

#include "components/nbody_component.h"
#include "modules/render_module.h"
#include "modules/assets_manager_module.h"
#include "systems/scene_system.h"
#include "systems/render_system.h"
#include "models/mesh_factory.h"
#include "modules/module_connector.h"
#include "systems/system_connector.h"

namespace aiko
{

    void NBodySystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_MODULE_REQUIRED(RenderModule, moduleConnector, m_renderModule);
        BIND_MODULE_REQUIRED(AssetsManagerModule, moduleConnector, m_assetManagerModule);
        BIND_SYSTEM_REQUIRED(RenderSystem, systemConnector, m_renderSystem);
        BIND_SYSTEM_REQUIRED(SceneSystem, systemConnector, m_sceneSystem);
    }

    void NBodySystem::init()
    {
        m_initShaderId = aiko->getComponentAssetAccess()->registerComputeShader("nbody_init");
        m_updateShaderId = aiko->getComponentAssetAccess()->registerComputeShader("nbody_update");
    }

    void NBodySystem::update()
    {
        const auto& objects = m_sceneSystem->getScene().getObjects();
        for (const auto& object : objects)
        {
            if (object == nullptr)
            {
                continue;
            }

            if (auto simulation = object->getComponent<NBodyComponent>())
            {
                updateSimulation(object, *simulation);
            }
        }
    }

    void NBodySystem::render()
    {
        const auto& objects = m_sceneSystem->getScene().getObjects();
        for (const auto& object : objects)
        {
            if (object == nullptr)
            {
                continue;
            }

            if (auto simulation = object->getComponent<NBodyComponent>())
            {
                renderSimulation(object, *simulation);
            }
        }
    }

    void NBodySystem::dispose()
    {
        destroyStates();
    }

    void NBodySystem::updateSimulation(GameObject* obj, NBodyComponent& simulation)
    {
        AIKO_UNUSED(obj);

        RuntimeState& state = getOrCreateState(&simulation);

        if (state.initialized == false)
        {
            const uint32_t count = simulation.getMaxBodies();

            state.positionMassBuffer.createVec4(count, nullptr, ComputeAccess::ReadWrite);
            state.velocityBuffer.createVec4(count, nullptr, ComputeAccess::ReadWrite);

            state.positionMassBufferNext.createVec4(count, nullptr, ComputeAccess::ReadWrite);
            state.velocityBufferNext.createVec4(count, nullptr, ComputeAccess::ReadWrite);

            state.positionMassCurrent = &state.positionMassBuffer;
            state.velocityCurrent = &state.velocityBuffer;

            state.positionMassWrite = &state.positionMassBufferNext;
            state.velocityWrite = &state.velocityBufferNext;

            state.initialized = true;
            state.initDispatched = false;
        }

        if (state.renderInitialized == false)
        {

            AssetId shaderId = m_assetManagerModule->getManager()->registerShader("gpu_billboard.vs", "model.fs");
            state.bodyMaterial.m_shader = &m_renderModule->getRenderer().resources().getShader(shaderId);
            state.bodyMaterial.m_baseColor = RED;
            state.bodyMaterial.m_lit = false;
            state.bodyMaterial.m_useVertexColor = true;

            state.renderInitialized = true;
        }

        if (simulation.consumeResetRequest() == true)
        {
            state.initDispatched = false;
        }
    }

    void NBodySystem::renderSimulation(GameObject* obj, NBodyComponent& simulation)
    {
        AIKO_UNUSED(obj);

        RuntimeState* state = tryGetState(&simulation);
        if (state == nullptr || simulation.isPlaying() == false)
        {
            return;
        }

        const uint32_t count = simulation.getMaxBodies();
        const vec3 emitterPos = obj->transform().position;

        if (count == 0)
        {
            return;
        }

        const float dt = Time::it().getDeltaTime();

        if (state->initDispatched == false)
        {
            ComputePass initPass{};
            initPass.buffers.push_back({ 0, &state->positionMassBuffer, ComputeAccess::ReadWrite });
            initPass.buffers.push_back({ 1, &state->velocityBuffer, ComputeAccess::ReadWrite });

            initPass.vec4Uniforms.push_back({
                "u_params",
                vec4(
                    float(count),
                    simulation.getInitialRadius(),
                    simulation.getInitialSpeed(),
                    simulation.getSoftening()
                )
            });

            initPass.vec4Uniforms.push_back({
                "u_origin",
                vec4(emitterPos.x, emitterPos.y, emitterPos.z, 0.0f)
            });

            initPass.vec4Uniforms.push_back({
                "u_initMode",
                vec4(float(static_cast<int>(simulation.getInitMode())), 0.0f, 0.0f, 0.0f)
            });

            initPass.vec4Uniforms.push_back({
                "u_gravity",
                vec4(
                    simulation.getGravitationalConstant().x,
                    simulation.getGravitationalConstant().y,
                    simulation.getGravitationalConstant().z,
                    simulation.getCentralMass()
                )
            });

            initPass.dispatch.groupsX = (count + 63) / 64;
            initPass.dispatch.groupsY = 1;
            initPass.dispatch.groupsZ = 1;

            m_renderSystem->dispatch(initPass, m_initShaderId);
            state->initDispatched = true;
        }

        ComputePass updatePass{};
        updatePass.buffers.push_back({ 0, state->positionMassCurrent, ComputeAccess::Read });
        updatePass.buffers.push_back({ 1, state->velocityCurrent, ComputeAccess::Read });

        updatePass.buffers.push_back({ 2, state->positionMassWrite, ComputeAccess::Write });
        updatePass.buffers.push_back({ 3, state->velocityWrite, ComputeAccess::Write });

        updatePass.vec4Uniforms.push_back({
            "u_params",
            vec4(
                dt,
                simulation.getTimeScale(),
                simulation.getSoftening(),
                float(count)
            )
        });

        updatePass.vec4Uniforms.push_back({
            "u_gravity",
            vec4(
                simulation.getGravitationalConstant().x,
                simulation.getGravitationalConstant().y,
                simulation.getGravitationalConstant().z,
                simulation.getCentralMass()
            )
        });

        updatePass.dispatch.groupsX = (count + 63) / 64;
        updatePass.dispatch.groupsY = 1;
        updatePass.dispatch.groupsZ = 1;

        m_renderSystem->dispatch(updatePass, m_updateShaderId);

        std::swap(state->positionMassCurrent, state->positionMassWrite);
        std::swap(state->velocityCurrent, state->velocityWrite);

        if (state->renderInitialized)
        {

            state->bodyMaterial.m_customVec4Uniforms["u_billboardParams"] = vec4(
                simulation.getRenderScale(),
                0.0f,
                0.0f,
                0.0f
            );

            GpuBillboardDrawDesc draw{};
            draw.material = &state->bodyMaterial;
            draw.positionBuffer = state->positionMassCurrent;
            draw.instanceCount = count;

            m_renderModule->getRenderer().drawBillboards(draw);

        }

    }

    NBodySystem::RuntimeState* NBodySystem::tryGetState(const NBodyComponent* cmp)
    {
        auto it = m_runtime.find(cmp);
        if (it != m_runtime.end())
        {
            return it->second.get();
        }
        return nullptr;
    }

    NBodySystem::RuntimeState& NBodySystem::getOrCreateState(const NBodyComponent* cmp)
    {
        if (RuntimeState* state = tryGetState(cmp))
        {
            return *state;
        }
        AikoUPtr<RuntimeState> state = std::make_unique<RuntimeState>();
        RuntimeState& ref = *state;
        m_runtime.emplace(cmp, std::move(state));
        return ref;
    }

    void NBodySystem::destroyStates()
    {
        for (auto& state : m_runtime)
        {
            state.second->positionMassBuffer.unload();
            state.second->velocityBuffer.unload();
            state.second->positionMassBufferNext.unload();
            state.second->velocityBufferNext.unload();
        }
        m_runtime.clear();
    }

}
