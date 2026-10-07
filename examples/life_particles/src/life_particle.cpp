#include "life_particle.h"

#include "layers/layer_context.h"
#include "types/draw_types.h"

#include <components/camera_component.h>
#include <core/random.h>
#include <magic_enum/magic_enum.hpp>
#include <models/camera.h>
#include <models/game_object.h>

#include <aiko_includes.h>

namespace lp
{

    namespace
    {
        aiko::Color particleColor(ParticleType type)
        {
            switch (type)
            {
                case ParticleType::Red:         return aiko::RED;
                case ParticleType::Green:       return aiko::GREEN;
                case ParticleType::Blue:        return aiko::BLUE;
            }
            AIKO_ASSERT(false, "Unknown particle type");
            return aiko::WHITE;
        }
    }

    void LifeParticles::init()
    {

        aiko::CameraComponent* camera = scene().createCamera(aiko::camera::CameraController::Fly);
        camera->getCamera().position = { 0.0f, 0.0f, 60.0f };

        aiko::MeshAsset asset = aiko::mesh::factory::generateMeshSphere( 7, 7);
        m_mesh.upload(asset);

        m_material.m_shaderId = context().assets().loadShader("model");
        m_material.m_lit = false;
        m_material.m_useVertexColor = true;
        m_material.m_baseColor = aiko::WHITE;

        constexpr float WorldHalfSize = 25.0f;

        auto getRandomPosition = []() -> aiko::vec3
        {
            return
            {
                aiko::utils::getRandomValue(-WorldHalfSize, WorldHalfSize),
                aiko::utils::getRandomValue(-WorldHalfSize, WorldHalfSize),
                0.0f
            };
        };

        auto getRandomType = []() -> ParticleType
        {
            auto rnd = aiko::utils::getRandomValue(0, magic_enum::enum_count<ParticleType>() - 1);
            return magic_enum::enum_value<ParticleType>(rnd);
        };

        for (uint64_t i = 0 ; i < c_particles_amount; ++i)
        {
            m_particles[i].position = getRandomPosition();
            m_particles[i].velocity = {0.0f, 0.0f, 0.0f};
            m_particles[i].type = getRandomType();
        }

    }

    void LifeParticles::update()
    {

    }

    void LifeParticles::render()
    {
        aiko::vector<aiko::InstanceData> data;
        data.reserve(m_particles.size());

        for (const Particle& particle : m_particles)
        {
            data.push_back(aiko::InstanceData
            {
                .position = particle.position,
                .rotation = aiko::vec3(0.0f),
                .scale = aiko::vec3(1.0f),
                .color = particleColor(particle.type),
            });
        }

        context().render().drawMeshInstanced(m_mesh, m_material, data.data(), static_cast<aiko::u32>(data.size()));
    }

}

