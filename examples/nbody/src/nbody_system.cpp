#include "nbody_system.h"

#include "models/mesh_factory.h"
#include "nbody_component.h"

#include <modules/assets_manager_module.h>
#include <modules/module_connector.h>
#include <systems/asset_system.h>
#include <systems/render_system.h>
#include <systems/scene_system.h>
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
            alignas(16) aiko::vec4 origin;
        };

        static_assert(sizeof(NBodyInitPushConstants) == 64);
        static_assert(sizeof(NBodyUpdatePushConstants) == 48);

    }

    void NBodySystem::connect(aiko::ModuleConnector* moduleConnector, aiko::SystemConnector* systemConnector)
    {
        BIND_MODULE_REQUIRED(aiko::AssetsManagerModule, moduleConnector, m_assetManagerModule);
        BIND_SYSTEM_REQUIRED(aiko::RenderSystem, systemConnector, m_renderSystem);
        BIND_SYSTEM_REQUIRED(aiko::SceneSystem, systemConnector, m_sceneSystem);
        BIND_SYSTEM_REQUIRED(aiko::AssetSystem, systemConnector, m_assetSystem);
    }

    void NBodySystem::init()
    {
        m_initShaderId = m_assetSystem->registerAndLoadAsset<aiko::ComputeShaderAsset>("nbody_init");
        m_updateShaderId = m_assetSystem->registerAndLoadAsset<aiko::ComputeShaderAsset>("nbody_update");
    }

    void NBodySystem::update()
    {
        for (NBodyComponent* simulation : m_sceneSystem->getScene().components<NBodyComponent>())
        {
            if (simulation == nullptr)
            {
                continue;
            }

            aiko::GameObject* object = simulation->getGameObject();
            AIKO_ASSERT(object != nullptr, "NBodyComponent is not attached to a GameObject");

            updateSimulation(object, *simulation);
        }
    }

    void NBodySystem::render()
    {
        for (NBodyComponent* simulation : m_sceneSystem->getScene().components<NBodyComponent>())
        {
            if (simulation == nullptr)
            {
                continue;
            }

            aiko::GameObject* object = simulation->getGameObject();
            AIKO_ASSERT(object != nullptr, "NBodyComponent is not attached to a GameObject");

            renderSimulation(object, *simulation);
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

            const aiko::ComputeBufferDesc positionBufferDesc
            {
                .format = aiko::ComputeBufferFormat::Vec4f,
                .count = count,
                .usage = aiko::ComputeBufferUsage::Storage | aiko::ComputeBufferUsage::Vertex,
            };

            const aiko::ComputeBufferDesc velocityBufferDesc
            {
                .format = aiko::ComputeBufferFormat::Vec4f,
                .count = count,
                .usage = aiko::ComputeBufferUsage::Storage,
            };

            const aiko::ComputeBufferDesc indexBufferDesc
            {
                .format = aiko::ComputeBufferFormat::Uint32,
                .count = count,
                .usage = aiko::ComputeBufferUsage::Storage | aiko::ComputeBufferUsage::Index,
            };
            state.indexBuffer.create(indexBufferDesc, nullptr);

            state.positionMassBuffer.create(positionBufferDesc, nullptr);
            state.velocityBuffer.create(velocityBufferDesc, nullptr);

            state.positionMassBufferNext.create(positionBufferDesc, nullptr);
            state.velocityBufferNext.create(velocityBufferDesc, nullptr);


            state.positionMassCurrent = &state.positionMassBuffer;
            state.velocityCurrent = &state.velocityBuffer;

            state.positionMassWrite = &state.positionMassBufferNext;
            state.velocityWrite = &state.velocityBufferNext;

            const aiko::ComputeBufferDesc indirectBufferDesc
            {
                .format = aiko::ComputeBufferFormat::Uint32,
                .count = 8,
                .usage =
                    aiko::ComputeBufferUsage::Storage |
                    aiko::ComputeBufferUsage::Indirect,
            };

            state.indirectBuffer.create(indirectBufferDesc, nullptr);

            state.initialized = true;
            state.initDispatched = false;
        }

        if (state.renderInitialized == false)
        {
            state.bodyMaterial.m_shaderId = m_assetManagerModule->getManager()->registerShader("nbody_gpuinst.vs", "model.fs");
            state.bodyMaterial.m_baseColor = aiko::RED;
            state.bodyMaterial.m_lit = true;
            state.bodyMaterial.m_useVertexColor = false;

            auto meshData = aiko::mesh::factory::generateMeshSphere(8, 8);
            state.bodyMesh.upload(meshData);

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

            state->positionMassCurrent = &state->positionMassBuffer;
            state->velocityCurrent = &state->velocityBuffer;
            state->positionMassWrite = &state->positionMassBufferNext;
            state->velocityWrite = &state->velocityBufferNext;

            aiko::ComputePass initPass{};
            initPass.buffers.push_back({ 0, &state->positionMassBuffer, aiko::ComputeAccess::ReadWrite });
            initPass.buffers.push_back({ 1, &state->velocityBuffer, aiko::ComputeAccess::ReadWrite });
            initPass.buffers.push_back({2, &state->indexBuffer, aiko::ComputeAccess::Write});
            initPass.buffers.push_back({3, &state->indirectBuffer, aiko::ComputeAccess::Write});

            const NBodyInitPushConstants initConstants
            {
                .params = aiko::vec4(
                    float(count),
                    simulation.getInitialRadius(),
                    simulation.getInitialSpeed(),
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
            ),
            .origin = aiko::vec4(
                emitterPos.x,
                emitterPos.y,
                emitterPos.z,
                0.0f
            ),
        };

        updatePass.setPushConstants(updateConstants);

        updatePass.dispatch.indirectBuffer = &state->indirectBuffer;
        updatePass.dispatch.indirectOffset = 5 * sizeof(uint32_t);

        m_renderSystem->dispatch(updatePass, m_updateShaderId);

        std::swap(state->positionMassCurrent, state->positionMassWrite);
        std::swap(state->velocityCurrent, state->velocityWrite);

        if (state->renderInitialized)
        {

            aiko::GpuInstanceDrawDesc draw;
            draw.mesh = &state->bodyMesh;
            draw.material = &state->bodyMaterial;
            draw.readBuffers.push_back({ 7, state->positionMassCurrent });
            draw.instanceCount = count;

            m_renderSystem->drawMeshInstancedGpu(draw);

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
            state.second->indexBuffer.unload();
            state.second->positionMassBuffer.unload();
            state.second->velocityBuffer.unload();
            state.second->positionMassBufferNext.unload();
            state.second->velocityBufferNext.unload();
            state.second->indirectBuffer.unload();
        }
        m_runtime.clear();
    }

}
