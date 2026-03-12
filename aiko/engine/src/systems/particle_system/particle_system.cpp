#include "particle_system.h"

#include <time/time.h>
#include "modules/render_module.h"
#include "modules/assets_manager_module.h"
#include "systems/scene_system.h"
#include "systems/render_system.h"
#include "components/particle_emitter_component.h"
#include "models/mesh_factory.h"

namespace aiko
{

    void ParticleSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_MODULE_REQUIRED(RenderModule, moduleConnector, m_renderModule);
        BIND_MODULE_REQUIRED(AssetsManagerModule, moduleConnector, m_assetManagerModule);
        BIND_SYSTEM_REQUIRED(RenderSystem, systemConnector, m_renderSystem);
        BIND_SYSTEM_REQUIRED(SceneSystem, systemConnector, m_sceneSystem);
    }

    void ParticleSystem::init()
    {
        m_initShaderId = aiko->getComponentAssetAccess()->registerComputeShader("particles_init");
        m_updateShaderId = aiko->getComponentAssetAccess()->registerComputeShader("particles_update");
    }

    void ParticleSystem::update()
    {
        const auto& objects = m_sceneSystem->getScene().getObjects();
        for (const auto& object : objects)
        {
            if (object == nullptr)
            {
                continue;
            }

            if (auto emitter = object->getComponent<ParticleEmitterComponent>())
            {
                updateEmitter(object, *emitter);
            }
        }
    }

    void ParticleSystem::render()
    {
        const auto& objects = m_sceneSystem->getScene().getObjects();
        for (const auto& object : objects)
        {
            if (object == nullptr)
            {
                continue;
            }

            if (auto emitter = object->getComponent<ParticleEmitterComponent>())
            {
                renderEmitter(object, *emitter);
            }
        }
    }

    void ParticleSystem::dispose()
    {
        destroyStates();
    }

    void ParticleSystem::updateEmitter(GameObject* obj, ParticleEmitterComponent& emitter)
    {
        AIKO_UNUSED(obj);

        RuntimeState& state = getOrCreateState(&emitter);

        if (state.initialized == false)
        {
            const uint32_t count = emitter.getMaxParticles();

            state.positionBuffer.createVec4(count, nullptr, ComputeAccess::ReadWrite);
            state.velocityBuffer.createVec4(count, nullptr, ComputeAccess::ReadWrite);
            state.lifeBuffer.createVec4(count, nullptr, ComputeAccess::ReadWrite);

            state.initialized = true;
            state.initDispatched = false;
            state.readbackRequested = false;
            state.readbackId = 0;
        }

        if (state.renderInitialized == false)
        {
            // Generate Quad and add billboard
            auto meshData = mesh::factory::generateCube();
            state.particleMesh.upload(meshData);

            AssetId shaderId = m_assetManagerModule->getManager()->registerShader("mesh_gpuinst.vs", "model.fs");
            state.particleMaterial.m_shader = &m_renderModule->getRenderer().resources().getShader(shaderId);
            state.particleMaterial.m_baseColor = WHITE;
            state.particleMaterial.m_lit = false;
            state.particleMaterial.m_useVertexColor = true;

            state.renderInitialized = true;
        }

        if (emitter.consumeResetRequest() == true)
        {
            state.initDispatched = false;
            state.readbackRequested = false;
            state.readbackId = InvalidReadbackId;
            state.spawnAccumulator = 0.0f;
            state.spawnThisFrame = 0;
            state.spawnCursor = 0;
        }
    }

    void ParticleSystem::renderEmitter(GameObject* obj, ParticleEmitterComponent& emitter)
    {
        AIKO_UNUSED(obj);

        RuntimeState* state = tryGetState(&emitter);
        if (state == nullptr || emitter.isPlaying() == false)
        {
            return;
        }

        const uint32_t count = emitter.getMaxParticles();
        const vec3 emitterPos = obj->transform().position;

        if (count == 0)
        {
            return;
        }

        const float dt = Time::it().getDeltaTime();

        state->spawnAccumulator += emitter.getSpawnRate() * dt;
        const auto requestedSpawn = static_cast<uint32_t>(state->spawnAccumulator);
        state->spawnThisFrame = std::min(requestedSpawn, count);
        state->spawnAccumulator -= static_cast<float>(requestedSpawn);

        const uint32_t spawnStart = state->spawnCursor;
        state->spawnCursor = (state->spawnCursor + state->spawnThisFrame) % count;
        state->spawnSeed += state->spawnThisFrame;

        if (state->initDispatched == false)
        {
            ComputePass initPass{};
            initPass.buffers.push_back({ 0, &state->positionBuffer, ComputeAccess::ReadWrite });
            initPass.buffers.push_back({ 1, &state->velocityBuffer, ComputeAccess::ReadWrite });
            initPass.buffers.push_back({ 2, &state->lifeBuffer, ComputeAccess::ReadWrite });

            initPass.vec4Uniforms.push_back({ "u_params", vec4(float(count), emitter.getLifetime(), emitter.getStartSpeed(), 0.0f)});
            initPass.vec4Uniforms.push_back({ "u_emitterPos", vec4(emitterPos.x, emitterPos.y, emitterPos.z, 0.0f)});

            initPass.dispatch.groupsX = (count + 63) / 64;
            initPass.dispatch.groupsY = 1;
            initPass.dispatch.groupsZ = 1;

            m_renderSystem->dispatch(initPass, m_initShaderId);
            state->initDispatched = true;
        }

        ComputePass updatePass{};
        updatePass.buffers.push_back({ 0, &state->positionBuffer, ComputeAccess::ReadWrite });
        updatePass.buffers.push_back({ 1, &state->velocityBuffer, ComputeAccess::ReadWrite });
        updatePass.buffers.push_back({ 2, &state->lifeBuffer, ComputeAccess::ReadWrite });

        updatePass.vec4Uniforms.push_back({
            "u_params",
            vec4(dt, emitter.getLifetime(), emitter.getStartSpeed(), 0.0f)
        });

        updatePass.vec4Uniforms.push_back({
            "u_emitterPos",
            vec4(emitterPos.x, emitterPos.y, emitterPos.z, 0.0f)
        });

        updatePass.vec4Uniforms.push_back({
            "u_spawnWindow",
            vec4(float(spawnStart), float(state->spawnThisFrame), float(count), 0.0f)
        });

        updatePass.vec4Uniforms.push_back({
            "u_spawnShape",
            vec4(float(static_cast<int>(emitter.getSpawnShape())), 0.0f, 0.0f, 0.0f)
        });

        updatePass.vec4Uniforms.push_back({
            "u_spawnData",
            vec4(
                emitter.getSpawnRadius(),
                emitter.getSpawnBoxExtents().x,
                emitter.getSpawnBoxExtents().y,
                emitter.getSpawnBoxExtents().z
            )
        });

        updatePass.vec4Uniforms.push_back({
            "u_direction",
            vec4(
                emitter.getDirection().x,
                emitter.getDirection().y,
                emitter.getDirection().z,
                emitter.getDirectionRandomness()
            )
        });

        updatePass.vec4Uniforms.push_back({
            "u_gravity",
            vec4(
                emitter.getGravity().x,
                emitter.getGravity().y,
                emitter.getGravity().z,
                0.0f
            )
        });

        updatePass.vec4Uniforms.push_back({
            "u_spawnSeed",
            vec4(float(state->spawnSeed), 0.0f, 0.0f, 0.0f)
        });

        updatePass.dispatch.groupsX = (count + 63) / 64;
        updatePass.dispatch.groupsY = 1;
        updatePass.dispatch.groupsZ = 1;

        m_renderSystem->dispatch(updatePass, m_updateShaderId);

        // Temporary debug readback of positions
        if( m_debugReadbackEnabled == true)
        {
            if (state->readbackRequested == false)
            {
                state->readbackId = m_nextReadbackId++;

                ComputeReadbackRequest req{};
                req.id = state->readbackId;
                req.buffer = &state->positionBuffer;
                req.byteSize = std::min<uint32_t>(count, 8) * sizeof(vec4);

                m_renderSystem->requestReadback(req);
                state->readbackRequested = true;
            }

            if (state->readbackRequested == true)
            {
                ComputeReadbackResult result{};
                if (m_renderSystem->pollReadback(result) == true)
                {
                    if (result.ready && result.id == state->readbackId)
                    {

                        const vec4* values = reinterpret_cast<const vec4*>(result.data.data());

                        logger::Log::info("particle[0] = (%f, %f, %f, %f)", values[0].x, values[0].y, values[0].z, values[0].w);
                        logger::Log::info("particle[1] = (%f, %f, %f, %f)", values[1].x, values[1].y, values[1].z, values[1].w);
                        logger::Log::info("particle[2] = (%f, %f, %f, %f)", values[2].x, values[2].y, values[2].z, values[2].w);

                        state->lastReadback = std::move(result);
                        state->readbackRequested = false;
                    }
                }
            }
        }

        if (state->renderInitialized)
        {

            state->particleMaterial.m_customVec4Uniforms["u_particleSizeLife"] = vec4(emitter.getStartLifeScale(), emitter.getEndLifeScale(), 0.0f, 0.0f);
            state->particleMaterial.m_customVec4Uniforms["u_particleStartColor"] = emitter.getStartLifeColor().toVec4();
            state->particleMaterial.m_customVec4Uniforms["u_particleEndColor"]   = emitter.getEndLifeColor().toVec4();

            GpuInstanceDrawDesc draw{};
            draw.mesh = &state->particleMesh;
            draw.material = &state->particleMaterial;
            draw.readBuffers.push_back({7, &state->positionBuffer});
            draw.readBuffers.push_back({8, &state->lifeBuffer});
            draw.instanceCount = count;

            m_renderModule->getRenderer().drawMeshInstancedGpu(draw);
        }

    }

    ParticleSystem::RuntimeState* ParticleSystem::tryGetState(const ParticleEmitterComponent* cmp)
    {
        auto it = m_runtime.find(cmp);
        if (it != m_runtime.end())
        {
            return it->second.get();
        }
        return nullptr;
    }

    ParticleSystem::RuntimeState& ParticleSystem::getOrCreateState(const ParticleEmitterComponent* cmp)
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

    void ParticleSystem::destroyStates()
    {
        for (auto& state : m_runtime)
        {
            state.second->positionBuffer.unload();
            state.second->velocityBuffer.unload();
            state.second->lifeBuffer.unload();
        }
        m_runtime.clear();
    }

}
