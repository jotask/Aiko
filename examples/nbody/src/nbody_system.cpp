#include "nbody_system.h"

#include "nbody_component.h"

#include <modules/render_module.h>
#include <modules/assets_manager_module.h>
#include <systems/scene_system.h>
#include <systems/render_system.h>
#include <modules/module_connector.h>
#include <systems/system_connector.h>

namespace nbody
{

    namespace
    {

        struct NBodyInitPushConstants
        {
            alignas(16) aiko::vec4 params;
            alignas(16) aiko::vec4 origin;
            alignas(16) aiko::vec4 initMode;
            alignas(16) aiko::vec4 gravity;
        };

        struct NBodyUpdatePushConstants
        {
            alignas(16) aiko::vec4 params;
            alignas(16) aiko::vec4 gravity;
        };

        static_assert(sizeof(NBodyInitPushConstants) == 64);
        static_assert(sizeof(NBodyUpdatePushConstants) == 32);

    }

    void NBodySystem::connect(aiko::ModuleConnector* moduleConnector, aiko::SystemConnector* systemConnector)
    {
        BIND_MODULE_REQUIRED(aiko::RenderModule, moduleConnector, m_renderModule);
        BIND_MODULE_REQUIRED(aiko::AssetsManagerModule, moduleConnector, m_assetManagerModule);
        BIND_SYSTEM_REQUIRED(aiko::RenderSystem, systemConnector, m_renderSystem);
        BIND_SYSTEM_REQUIRED(aiko::SceneSystem, systemConnector, m_sceneSystem);
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

    void NBodySystem::updateSimulation(aiko::GameObject* obj, NBodyComponent& simulation)
    {
        AIKO_UNUSED(obj);

        RuntimeState& state = getOrCreateState(&simulation);

        if (state.initialized == false)
        {
            const uint32_t count = simulation.getMaxBodies();

            const aiko::ComputeBufferDesc bufferDesc
            {
                .format = aiko::ComputeBufferFormat::Vec4f,
                .count = count,
                .usage = aiko::ComputeBufferUsage::Storage,
            };

            state.positionMassBuffer.create(bufferDesc, nullptr);
            state.velocityBuffer.create(bufferDesc, nullptr);
            state.positionMassBufferNext.create(bufferDesc, nullptr);
            state.velocityBufferNext.create(bufferDesc, nullptr);

            state.positionMassCurrent = &state.positionMassBuffer;
            state.velocityCurrent = &state.velocityBuffer;

            state.positionMassWrite = &state.positionMassBufferNext;
            state.velocityWrite = &state.velocityBufferNext;

            state.initialized = true;
            state.initDispatched = false;
        }

        if (state.renderInitialized == false)
        {
            state.bodyMaterial.m_shaderId = m_assetManagerModule->getManager()->registerShader("gpu_billboard.vs", "model.fs");
            state.bodyMaterial.m_baseColor = aiko::RED;
            state.bodyMaterial.m_lit = false;
            state.bodyMaterial.m_useVertexColor = true;

            state.renderInitialized = true;
        }

        if (simulation.consumeResetRequest() == true)
        {
            state.initDispatched = false;
        }
    }

    void NBodySystem::renderSimulation(aiko::GameObject* obj, NBodyComponent& simulation)
    {

        RuntimeState* state = tryGetState(&simulation);
        if (state == nullptr || simulation.isPlaying() == false)
        {
            return;
        }

        const uint32_t count = simulation.getMaxBodies();
        const aiko::vec3 emitterPos = obj->transform().position;

        if (count == 0)
        {
            return;
        }

        if (state->initDispatched == false)
        {
            aiko::ComputePass initPass{};
            initPass.buffers.push_back({ 0, &state->positionMassBuffer, aiko::ComputeAccess::ReadWrite });
            initPass.buffers.push_back({ 1, &state->velocityBuffer, aiko::ComputeAccess::ReadWrite });

            const NBodyInitPushConstants initConstants
            {
                .params = aiko::vec4(
                    simulation.getTimeScale(),
                    simulation.getSoftening(),
                    float(count),
                    0.0f
                ),
                .origin = aiko::vec4(
                    emitterPos.x,
                    emitterPos.y,
                    emitterPos.z,
                    0.0f
                ),
                .initMode = aiko::vec4(
                    float(static_cast<int>(simulation.getInitMode())),
                    0.0f,
                    0.0f,
                    0.0f
                ),
                .gravity = aiko::vec4(
                    simulation.getGravitationalConstant().x,
                    simulation.getGravitationalConstant().y,
                    simulation.getGravitationalConstant().z,
                    simulation.getCentralMass()
                )
            };

            initPass.setPushConstants(initConstants);

            initPass.dispatch.groupsX = (count + 63) / 64;
            initPass.dispatch.groupsY = 1;
            initPass.dispatch.groupsZ = 1;

            m_renderSystem->dispatch(initPass, m_initShaderId);
            state->initDispatched = true;
        }

        aiko::ComputePass updatePass{};
        updatePass.buffers.push_back({ 0, state->positionMassCurrent, aiko::ComputeAccess::Read });
        updatePass.buffers.push_back({ 1, state->velocityCurrent, aiko::ComputeAccess::Read });

        updatePass.buffers.push_back({ 2, state->positionMassWrite, aiko::ComputeAccess::Write });
        updatePass.buffers.push_back({ 3, state->velocityWrite, aiko::ComputeAccess::Write });

        const NBodyUpdatePushConstants updateConstants
        {
            .params = aiko::vec4(
                simulation.getTimeScale(),
                simulation.getSoftening(),
                float(count),
                0.0f
            ),
            .gravity = aiko::vec4(
                simulation.getGravitationalConstant().x,
                simulation.getGravitationalConstant().y,
                simulation.getGravitationalConstant().z,
                simulation.getCentralMass()
            )
        };

        updatePass.setPushConstants(updateConstants);

        updatePass.dispatch.groupsX = (count + 63) / 64;
        updatePass.dispatch.groupsY = 1;
        updatePass.dispatch.groupsZ = 1;

        m_renderSystem->dispatch(updatePass, m_updateShaderId);

        std::swap(state->positionMassCurrent, state->positionMassWrite);
        std::swap(state->velocityCurrent, state->velocityWrite);

        if (state->renderInitialized)
        {

            state->bodyMaterial.m_customVec4Uniforms["u_billboardParams"] = aiko::vec4(
                simulation.getRenderScale(),
                0.0f,
                0.0f,
                0.0f
            );

            aiko::GpuBillboardDrawDesc draw{};
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
        aiko::AikoUPtr<RuntimeState> state = std::make_unique<RuntimeState>();
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
